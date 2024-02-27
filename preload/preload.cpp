#include <inttypes.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hip/hip_runtime.h"

#include "hip/hip_runtime_api.h"
#include <link.h>
#include <vector>
#include <map>
#include "INIReader.h"
#include <iostream>
#include <stdio.h>
#include <mutex>
using namespace std;
#include <chrono>


#define HIP_ASSERT(x) (assert((x)==hipSuccess))
/*typedef float SECS_t;
inline long long get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

// Returns the number of seconds elapsed between the two specified times
inline SECS_t elapsed_time(long long start_time, long long end_time)
{
    return ((SECS_t) (end_time - start_time)) / ((SECS_t)(1000 * 1000));
}*/

/*
   typedef struct dim3 {
   uint32_t x;  ///< x
   uint32_t y;  ///< y
   uint32_t z;  ///< z
   } dim3;



// http://rocm-developer-tools.github.io/HIP/hcc__detail_2hip__vector__types_8h_source.html
typedef struct uint3 {
unsigned x;  ///< x
unsigned y;  ///< y
unsigned z;  ///< z
} uint3;
*/

int old_kernarg_size;
int old_kernarg_num;

FILE * fdata;

typedef struct kernel_launch_tracker{
    uint32_t * data;
    uint32_t * data_h;
    uint32_t avail_size;
    uint32_t data_size;
    uint32_t status;

    uint32_t kid;
    uint32_t num_records;
    uint32_t num_branches;
}kernel_launch_tracker;

std::vector<kernel_launch_tracker*> mempool;

void * fatbin_data;
#define ElfW(type)        _ElfW (Elf, __ELF_NATIVE_CLASS, type)
#define _ElfW(e,w,t)        _ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)        e##w##t

typedef void** (*registerFatBinary_t ) (
        const void * data);



registerFatBinary_t realRegisterFatBinary =0 ;
struct __CudaFatBinaryWrapper {
    unsigned int magic;
    unsigned int version;
    void*        binary;
    void*        dummy1;
};

FILE * fdebug;
extern "C" void** __hipRegisterFatBinary(void* data)
{
    if(realRegisterFatBinary == 0){
        realRegisterFatBinary = (registerFatBinary_t) dlsym(RTLD_NEXT,"__hipRegisterFatBinary");   
    }

    __CudaFatBinaryWrapper* fbwrapper = reinterpret_cast<__CudaFatBinaryWrapper*>(data);
    fprintf(fdebug,"data %lx : %s %lx\n", (uint64_t) data, (char * ) data, (uint64_t) fbwrapper->binary); 
    return realRegisterFatBinary( data );

}

typedef void (*registerFunc_t ) (
        void** modules,
        const void*  hostFunction,
        char*        deviceFunction,
        const char*  deviceName,
        unsigned int threadLimit,
        uint3*       tid,
        uint3*       bid,
        dim3*        blockDim,
        dim3*        gridDim,
        int*         wSize);

typedef struct config {
    uint32_t kernarg_size ;
    uint32_t kernarg_num ;
    uint32_t branch_count ;
    uint32_t kid;
    std::string name;
}config;
std::map<const void * , config* > FuncLookup;
std::map<std::string, config *> metaLookup; 
std::map<int ,int > testMap;
registerFunc_t realRegisterFunction;
int g_kid = 0;

extern "C" void __hipRegisterFunction(
        void** modules,
        const void*  hostFunction,
        char*        deviceFunction,
        const char*  deviceName,
        unsigned int threadLimit,
        uint3*       tid,
        uint3*       bid,
        dim3*        blockDim,
        dim3*        gridDim,
        int*         wSize){
    fprintf(fdebug,"modules = %p, hostFunciton = %p, devceFunciton = %s, deviceName = %s\n",modules,hostFunction, deviceFunction, deviceName);

    if(realRegisterFunction == 0){
        realRegisterFunction = (registerFunc_t) dlsym(RTLD_NEXT,"__hipRegisterFunction");   
        INIReader reader("config.ini");
        if(reader.ParseError() !=0){
            std::cerr << "Can't load " << std::string("config.ini")  << std::endl;
            exit(-1);

        }
        auto &sections = reader.Sections();
        for (auto section : sections){
            config * c = new config();
            std::string kname = reader.Get(section,"kernel_name","");
            c->kernarg_size = reader.GetInteger(section,"kernarg_size",-1);
            c->kernarg_num  = reader.GetInteger(section,"kernarg_num",-1);
            c->branch_count = reader.GetInteger(section,"branch_count",-1);
            c->name = kname;
            c->kid = g_kid;
            g_kid ++;
            fprintf(fdebug,"kernal name = %s\n",kname.c_str());
            metaLookup[kname] = c;
        }

    }
    realRegisterFunction(modules,hostFunction,deviceFunction,deviceName,threadLimit,tid,bid,blockDim,gridDim,wSize);
    std::string kname = std::string(deviceName);
    if( metaLookup.find(kname) != metaLookup.end()){
        FuncLookup[hostFunction] = metaLookup[kname];
    }
    return;
}

//hipamd/src/hip_module.cpp
//
//

typedef uint32_t (*launch_t)(const void *hostFunction,
        dim3 gridDim,
        dim3 blockDim,
        void** args,
        size_t sharedMemBytes,
        hipStream_t stream);


launch_t realLaunch;


void print_result_v2(uint32_t * records, const char * kname ,uint32_t num_warps, uint32_t num_branches){
    fprintf(fdebug,"num_records = %u,%s\n",num_warps * (num_branches+2),kname);
    uint32_t stride = (num_branches+2)*2;

    fprintf(fdata,"num_records = %u,%s\n",num_warps,kname);
    for ( uint32_t index =0 ; index < 256 ; index ++){
      fprintf(fdata,"%u: %x\n",index,records[index]);
    }
    fprintf(fdata,"done\n");
}


//uint32_t * data, * data_h , data_size;
//uint32_t no_records;
uint32_t launch_id = 0;
extern "C" hipError_t hipLaunchKernel(const void *hostFunction,
        dim3 gridDim,
        dim3 blockDim,
        void** args,
        size_t sharedMemBytes,
        hipStream_t stream)
{

    if(realLaunch ==0){
        realLaunch = (launch_t)  dlsym(RTLD_NEXT, "hipLaunchKernel");
    }

    config * c = FuncLookup[hostFunction];
    if(c){


        uint32_t warps_per_block =  ( blockDim.x * blockDim.y * blockDim.z + 63 ) / 64;
        uint32_t no_warps = gridDim.x * gridDim.y * gridDim.z * warps_per_block;

        uint32_t num_branches = c->branch_count; // TODO:

        uint32_t size_per_warp = (num_branches * 2 * sizeof(uint32_t) + 2 * sizeof(uint64_t)); // 2 for counters for each branch , 2 for timestamp
        uint32_t data_size = no_warps * size_per_warp * 64;
        uint32_t no_records = data_size / sizeof(uint32_t);

        kernel_launch_tracker * klt_ptr;
        bool found = false;
        /*for ( int i =0; i < mempool.size() ; i ++){
          klt_ptr = mempool[i]; 
          fprintf(fdebug,"Iterating  memory pool at 0x%llx, size = %u\n",klt_ptr->data_h, data_size);
          if(mempool[i]->status == 2 && mempool[i]->data_size >= klt_ptr->avail_size){
          klt_ptr = mempool[i]; 
          found = true;
          fprintf(fdebug,"Reusing memory at 0x%llx, size = %u\n",klt_ptr->data_h, data_size);
          break;
          }
          }*/
        hipError_t hip_ret;
        if(found == false){
            fprintf(fdebug,"Can't reuse memory , allocating new memory of size %u\n",data_size);
            klt_ptr = (kernel_launch_tracker *)  malloc(sizeof(kernel_launch_tracker));
            mempool.push_back(klt_ptr);
            klt_ptr->data_h = (uint32_t * ) malloc(data_size);
            HIP_ASSERT(hipMalloc((void **) & klt_ptr->data , data_size  ));
            klt_ptr->avail_size = data_size;
        }
        klt_ptr->data_size = data_size;
        klt_ptr->status = 1;
        klt_ptr->kid = c->kid ;
        klt_ptr->num_records = no_records ;
        klt_ptr->num_branches =num_branches ;

        //if(launch_id == 0){
        memset(klt_ptr->data_h,0x0, data_size);
        HIP_ASSERT(hipMemcpy(klt_ptr->data,klt_ptr->data_h,data_size,hipMemcpyHostToDevice));
        //}
        int new_kernarg_vec_size = (c->kernarg_num + 6) * 8 ;
        void ** newArgs = (void **) malloc(new_kernarg_vec_size);
        memcpy(newArgs, args , c->kernarg_num *8);

        fprintf(fdebug,"hip Launch Kernel Called, hostFunction = %p, kernel name = %s\n",hostFunction,c->name.c_str());
        fprintf(fdebug,"Grid(x,y,z)=(%u,%u,%u) Block(x,y,z)=(%u,%u,%u)\n", gridDim.x , gridDim.y , gridDim.z ,blockDim.x , blockDim.y , blockDim.z);
        fprintf(fdebug,"WARPS_PER_BLOCK = %u, NO_WARPS = %u, NUM_BRANCHES = %u\n", warps_per_block, no_warps, num_branches);
        fprintf(fdebug,"data_h = 0x%p , data = 0x%p\n", klt_ptr->data_h, klt_ptr->data);

        *(newArgs+c->kernarg_num) = &(klt_ptr->data);

        *(newArgs+c->kernarg_num+1) = &warps_per_block;
        *(newArgs+c->kernarg_num+2) = &(gridDim.x);
        *(newArgs+c->kernarg_num+3) = &(gridDim.y);
        *(newArgs+c->kernarg_num+4) = &(blockDim.x);
        *(newArgs+c->kernarg_num+5) = &(blockDim.y);

        //typedef std::chrono::high_resolution_clock Clock;
        typedef std::chrono::steady_clock Clock;
        auto t1 = Clock::now();
        realLaunch(hostFunction,gridDim,blockDim, newArgs, sharedMemBytes, stream);
        HIP_ASSERT(hipStreamSynchronize(stream));
        auto t2 = Clock::now();
        //std::chrono::duration<double> elapsed_seconds = t2 - t1;
        int clock_rate = 0;
        HIP_ASSERT(hipDeviceGetAttribute (&clock_rate, hipDeviceAttributeClockRate, 0 ));
        cout << "Elapsed duration = " << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() << " clockRate = " <<  clock_rate << "\n";
        fprintf(fdebug,"copying to 0x%p from 0x%p\n", klt_ptr->data_h,klt_ptr->data);

        HIP_ASSERT(hipMemcpy(klt_ptr->data_h,klt_ptr->data,klt_ptr->data_size,hipMemcpyDeviceToHost));
        //fprintf(fdata,"%s\n",c->name.c_str());
        //print_result (klt_ptr->data_h, klt_ptr->kid , klt_ptr -> num_records, klt_ptr->num_branches);
        print_result_v2 (klt_ptr->data_h, c->name.c_str() , no_warps, klt_ptr->num_branches);
        klt_ptr->status=2;
        fprintf(fdebug,"hip Launch Kernel Ended\n\n");
        launch_id ++;
        return hipSuccess;
    }else{
        fprintf(fdebug,"config not found, using old args\n");
        realLaunch(hostFunction,gridDim,blockDim, args, sharedMemBytes, stream);

        return hipSuccess;
    }
}


// hip_memory.cpp
//
typedef  hipError_t (* malloc_t ) ( void ** ptr , size_t sizeBytes);
malloc_t realMalloc;
hipError_t hipMalloc(void** ptr, size_t sizeBytes){
    if(realMalloc == 0){
        realMalloc = (malloc_t) dlsym(RTLD_NEXT,"hipMalloc");   
    }
    return realMalloc(ptr,sizeBytes); 
}


typedef hipError_t (* memcpy_t) (void* dst, const void* src, size_t sizeBytes, hipMemcpyKind kind);
memcpy_t realMemcpy;
/*
   hipError_t hipMemcpy(void* dst, const void* src, size_t sizeBytes, hipMemcpyKind kind){
   if(realMemcpy ==0){
   realMemcpy = (memcpy_t) dlsym(RTLD_NEXT,"hipMemcpy");
   }
   printf("hipMemcpy dst = %p , src = %p\n", dst,src);
   return realMemcpy(dst,src,sizeBytes,kind);
   }*/



int gct=0;
std::mutex g_mtx;
__attribute__((constructor)) static void setup(void){
    realLaunch = 0;
    //realRegisterFunction = 0;
    realMalloc = 0;
    realMemcpy =0;
    /*if(realRegisterFatBinary == 0){
      realRegisterFatBinary = (registerFatBinary_t) dlsym(RTLD_NEXT,"__hipRegisterFatBinary");   
      }

      FILE * f_fatbin = fopen("/home/wuxx1279/krt/vectorAdd/bundle.txt","r");
      fseek(f_fatbin,0,SEEK_END);
      size_t fatbin_size = ftell(f_fatbin);
      fseek(f_fatbin,0,SEEK_SET);

      fatbin_data = aligned_alloc(0x1000,fatbin_size);
      fread(fatbin_data,1,fatbin_size,f_fatbin);
      fclose(f_fatbin);
      */
    g_mtx.lock();
    if(gct ==0 ){
        printf("calling setup, gct = %d\n",gct++);
        fdata = fopen("measure_result.txt","w");
        fdebug = fopen("preload_debug.txt","w");
    }
    g_mtx.unlock();
}


__attribute__((destructor)) static void fini(void){
    kernel_launch_tracker * klt;

    for(std::vector<kernel_launch_tracker*>::iterator it = mempool.begin(); it != mempool.end(); ){
        klt = *it;
        if(klt->status == 1){
            HIP_ASSERT(hipFree(klt->data));
            free(klt->data_h);
            klt->status = 0;
        }
        it = mempool.erase(it);
    }
    fclose(fdata);
    fclose(fdebug);
}

