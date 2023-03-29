
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hip/hip_runtime.h"

#include "hip/hip_runtime_api.h"
#include "amd_comgr.h"
#include <link.h>
#include <vector>
#include <map>
#include "INIReader.h"
#include <iostream>
#include <stdio.h>
#include <mutex>
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
typedef struct klt_result_t{
    uint32_t * records;
    uint32_t kid;
    uint32_t num_records;
    uint32_t num_branches;
}klt_result_t;


typedef struct kernel_launch_tracker{
    uint32_t * data;
    uint32_t * data_h;
    uint32_t avail_size;
    uint32_t data_size;
    uint32_t status;
    klt_result_t result;
}kernel_launch_tracker;

std::vector<kernel_launch_tracker*> mempool;

void * fatbin_data;
#define ElfW(type)        _ElfW (Elf, __ELF_NATIVE_CLASS, type)
#define _ElfW(e,w,t)        _ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)        e##w##t

typedef void** (*registerFatBinary_t ) (
        const void * data);


/*
   registerFatBinary_t realRegisterFatBinary =0 ;
   struct __CudaFatBinaryWrapper {
   unsigned int magic;
   unsigned int version;
   void*        binary;
   void*        dummy1;
   };

   extern "C" void** __hipRegisterFatBinary(void* data)
   {
   __CudaFatBinaryWrapper* fbwrapper = reinterpret_cast<__CudaFatBinaryWrapper*>(data);
   __CudaFatBinaryWrapper newwrapper;
   memcpy(&newwrapper,fbwrapper,sizeof(__CudaFatBinaryWrapper));
   printf("data %lx : %s %lx\n", (uint64_t) data, (char * ) data, (uint64_t) fbwrapper->binary); 
   newwrapper.binary = fatbin_data;
   return realRegisterFatBinary( &newwrapper );

   }*/

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
}config;
std::map<const void * , config* > FuncLookup;
std::map<std::string, config *> metaLookup; 
std::map<int ,int > testMap;
registerFunc_t realRegisterFunction;
int g_kid = 0;

FILE * fdebug;
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

void print_result(uint32_t * records, uint32_t kid ,uint32_t num_records, uint32_t num_branches){
    fprintf(fdata,"num_records = %u\n",num_records);
    fprintf(fdebug,"num_records = %u\n",num_records);
    uint32_t m_stride = (num_branches+2) * 2;
    for (uint32_t i =0; i < num_records ;i += m_stride){
        uint32_t j =0;
        for(; j < num_branches ; j++){
#ifdef PRINT_DETAIL
            fprintf(fdata,"\tK%d:%u %u\n",kid,records[i+j*2],records[i+j*2+1]);
#endif
        }
        uint64_t * mem_time = (uint64_t * ) & records [i+j*2];
        fprintf(fdata,"%u: %lu %lu\n",kid,mem_time[0],mem_time[1]);
    }
    fprintf(fdata,"done\n");
}


//uint32_t * data, * data_h , data_size;
//uint32_t no_records;
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

    fprintf(fdebug,"hip Launch Kernel Called, hostFunction = %p\n",hostFunction);
    fprintf(fdebug,"x y z = %u %u %u\n",gridDim.x , gridDim.y , gridDim.z);
    fprintf(fdebug,"x y z = %u %u %u\n",blockDim.x , blockDim.y , blockDim.z);

    config * c = FuncLookup[hostFunction];
    if(c){
        uint32_t warps_per_block =  ( blockDim.x * blockDim.y * blockDim.z + 63 ) / 64;
        uint32_t no_warps = gridDim.x * gridDim.y * gridDim.z * warps_per_block;

        uint32_t num_branches = c->branch_count; // TODO:

        uint32_t size_per_warp = (num_branches * 2 * sizeof(uint32_t) + 2 * sizeof(uint64_t)); // 2 for counters for each branch , 2 for timestamp
        uint32_t data_size = no_warps * size_per_warp;
        uint32_t no_records = data_size / sizeof(uint32_t);

        kernel_launch_tracker * klt_ptr;
        bool found = false;
        for ( int i =0; i < mempool.size() ; i ++){
            if(mempool[i]->status == 2 && mempool[i]->data_size >= klt_ptr->avail_size){
                klt_ptr = mempool[i]; 
                found = true;
                break;
            }
        }
        if(found == false){
            klt_ptr = (kernel_launch_tracker *)  malloc(sizeof(kernel_launch_tracker));
            mempool.push_back(klt_ptr);
            klt_ptr->data_h = (uint32_t * ) malloc(data_size);
            hipMalloc((void **) & klt_ptr->data , data_size  );
            klt_ptr->result.records = klt_ptr->data_h;
            klt_ptr->avail_size = data_size;
        }
        klt_ptr->data_size = data_size;
        klt_ptr->status = 1;
        klt_ptr->result.kid = c->kid ;
        klt_ptr->result.num_records = no_records ;
        klt_ptr->result.num_branches =num_branches ;

        memset(klt_ptr->data_h,0, data_size);
        hipMemcpy(klt_ptr->data,klt_ptr->data_h,data_size,hipMemcpyHostToDevice);

        int new_kernarg_vec_size = (c->kernarg_num + 6) * 8 ;
        void ** newArgs = (void **) malloc(new_kernarg_vec_size);
        memcpy(newArgs, args , c->kernarg_num *8);


        *(newArgs+c->kernarg_num) = &(klt_ptr->data);
        *(newArgs+c->kernarg_num+1) = &warps_per_block;
        *(newArgs+c->kernarg_num+2) = &(gridDim.x);
        *(newArgs+c->kernarg_num+3) = &(gridDim.y);
        *(newArgs+c->kernarg_num+4) = &(blockDim.x);
        *(newArgs+c->kernarg_num+5) = &(blockDim.y);
        realLaunch(hostFunction,gridDim,blockDim, newArgs, sharedMemBytes, stream);

        kernel_launch_tracker * klt = (kernel_launch_tracker *) klt_ptr;
        klt_result_t * klt_result = (klt_result_t*) &(klt->result);
        hipMemcpy(klt_ptr->data_h,klt_ptr->data,klt_ptr->data_size,hipMemcpyDeviceToHost);
        print_result (klt_result->records, klt_result->kid , klt_result -> num_records, klt_result->num_branches);
        klt_ptr->status=2;

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
            hipFree(klt->data);
            free(klt->data_h);
            klt->status = 0;
        }
        it = mempool.erase(it);
    }
    fclose(fdata);
    fclose(fdebug);
}

