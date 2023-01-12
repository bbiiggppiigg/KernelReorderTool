
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hip/hip_runtime.h"

#include "hip/hip_runtime_api.h"
#include "amd_comgr.h"
#include <link.h>
#include <vector>
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



registerFunc_t realRegisterFunction;
//include/hip/amd_detail/amd_hip_runtime.h
//hipamd/include/hip/amd_detail/amd_hip_runtime.h

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
    printf("modules = %p, hostFunciton = %p, devceFunciton = %s, deviceName = %s\n",modules,hostFunction, deviceFunction, deviceName);
    if(realRegisterFunction == 0){
        realRegisterFunction = (registerFunc_t) dlsym(RTLD_NEXT,"__hipRegisterFunction");   
    }
    realRegisterFunction(modules,hostFunction,deviceFunction,deviceName,threadLimit,tid,bid,blockDim,gridDim,wSize);

    return;
}




/*extern "C" void** __hipRegisterFatBinary(const void* data){
  printf("register fat binary called, data = %s\n", data );
  return 0;
  }*/



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
    uint32_t m_stride = (num_branches+2) * 2;
    for (uint32_t i =0; i < num_records ;i += m_stride){
        uint32_t j =0;
        for(; j < num_branches ; j++){
            printf("\tK%d:%u %u\n",kid,records[i+j*2],records[i+j*2+1]);
        }
        uint64_t * mem_time = (uint64_t * ) & records [i+j*2];
        printf("K%d#: %lu %lu\n",kid,mem_time[0],mem_time[1]);
    }
}

void callback (hipStream_t stream, hipError_t status, void *userData){
    kernel_launch_tracker * klt = (kernel_launch_tracker *) userData;
    klt_result_t * klt_result = (klt_result_t*) klt;

    hipMemcpy(klt->data_h,klt->data,klt->data_size,hipMemcpyDeviceToHost);
    print_result (klt_result->records, klt_result->kid , klt_result -> num_records, klt_result->num_branches);
    printf("kernel completed\n");
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

    printf("hip Launch Kernel Called, hostFunction = %p, stream = %u\n",hostFunction,stream);
    printf("x y z = %u %u %u\n",gridDim.x , gridDim.y , gridDim.z);
    printf("x y z = %u %u %u\n",blockDim.x , blockDim.y , blockDim.z);
    uint32_t warps_per_block =  ( blockDim.x * blockDim.y * blockDim.z + 63 ) / 64;
    uint32_t no_warps = warps_per_block * gridDim.x * gridDim.y * gridDim.z * warps_per_block;
    uint32_t num_branches = 1; // TODO:
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
        klt_ptr->result.records = klt_ptr->data;
        klt_ptr->avail_size = data_size;
    }
    klt_ptr->data_size = data_size;
    klt_ptr->status = 1;
    klt_ptr->result.kid =0 ;
    klt_ptr->result.num_records = no_records ;
    klt_ptr->result.num_branches =num_branches ;


    memset(klt_ptr->data_h,0, data_size);
    hipMemcpy(klt_ptr->data,klt_ptr->data_h,data_size,hipMemcpyHostToDevice);

    void ** newArgs = (void **) malloc(48);
    memcpy(newArgs, args , 40);
    



    //print_result(data_h, 0 , no_records, 1 );

    /*
    void ** ptrI = (void **)(args);

    printf("args = %p, %p \n", *(args+3),*(args+4));



    uint32_t width = * ((uint32_t *) *(ptrI+3));
    uint32_t height = * ((uint32_t *) *(ptrI+4));


    for( int i = 0; i < 5; i++){
        printf("[%*p]:%p %lx\n",16, ptrI+i, *(ptrI+i) , * ((uint64_t * ) *(ptrI+i)) );
    }
    printf(" width = %x, height = %x\n",  width , height);
    */

    *(newArgs+5) = &(klt_ptr->data);
    hipStreamAddCallback(stream,callback , klt ,0);

    realLaunch(hostFunction,gridDim,blockDim, newArgs, sharedMemBytes, stream);
    return hipSuccess;
}


extern "C" hipError_t hipSetupArgument(
        const void *arg,
        size_t size,
        size_t offset){
    printf("testing setup argument\n");
    exit(-1);
    return hipSuccess;
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

hipError_t hipMemcpy(void* dst, const void* src, size_t sizeBytes, hipMemcpyKind kind){
    if(realMemcpy ==0){
        realMemcpy = (memcpy_t) dlsym(RTLD_NEXT,"hipMemcpy");
    }
    printf("hipMemcpy dst = %p , src = %p\n", dst,src);
    return realMemcpy(dst,src,sizeBytes,kind);
}


hipError_t ihipLaunchKernelCommand(void*& command, hipFunction_t f,
        uint32_t globalWorkSizeX, uint32_t globalWorkSizeY,
        uint32_t globalWorkSizeZ, uint32_t blockDimX, uint32_t blockDimY,
        uint32_t blockDimZ, uint32_t sharedMemBytes,
        void* queue, void** kernelParams, void** extra,
        hipEvent_t startEvent = nullptr, hipEvent_t stopEvent = nullptr,
        uint32_t flags = 0, uint32_t params = 0, uint32_t gridId = 0,
        uint32_t numGrids = 0, uint64_t prevGridSum = 0,
        uint64_t allGridSum = 0, uint32_t firstDevice = 0) {
    exit(-1);
    return hipSuccess;
}




__attribute__((constructor)) static void setup(void){
    realLaunch = 0;
    realRegisterFunction = 0;
    realMalloc = 0;
    realMemcpy =0;
    if(realRegisterFatBinary == 0){
        realRegisterFatBinary = (registerFatBinary_t) dlsym(RTLD_NEXT,"__hipRegisterFatBinary");   
    }

    FILE * f_fatbin = fopen("/home/wuxx1279/krt/vectorAdd/bundle.txt","r");
    fseek(f_fatbin,0,SEEK_END);
    size_t fatbin_size = ftell(f_fatbin);
    fseek(f_fatbin,0,SEEK_SET);

    fatbin_data = aligned_alloc(0x1000,fatbin_size);
    fread(fatbin_data,1,fatbin_size,f_fatbin);
    fclose(f_fatbin);

    printf("calling setup\n");
}


__attribute__((destructor)) static void fini(void){
    
    //hipMemcpy(data_h,data,data_size,hipMemcpyDeviceToHost);
    //print_result(data_h, 0 , no_records, 1 );
}





/*
   float * ptrA;
   float * ptrB;
   float * ptrC;


   width = *((uint32_t * ) (args[3]));
   height = *((uint32_t * ) (args[4]));

   ptrA = ((float *)( args[0]));
   ptrB = ((float *)( args[1]));
   ptrC = ((float *)( args[2]));
   printf("width = %u height = %u\n",width,height);
   printf("ptr A = %p, B = %p, C =  %p %p %p\n",  *((float * ) ptrA),ptrB,ptrC,args[3],args[4]);




*/

/*printf("args %p = %p %p %p %u %u\n",args,(args+8), (args+16) ,(args+24), *((uint32_t * )args) , *((uint32_t * )args[4]) ); 
  printf("args = %p %p %p\n",*((char *)args),*((char * )args+8), *((char  *)args+16)); 

 * ((uint32_t * ) args[3])  = 4;*/

/*float * testB;

  float * testB_h;
  testB_h = (float *) malloc(sizeof(float) * 256);
  for( int i =0 ; i < 256 ; i++){
  testB_h[i] = 256-2*i;
  }
  hipMalloc((void **) &testB, 256 * sizeof(float) );
  hipMemcpy(testB,testB_h,256,hipMemcpyHostToDevice);


  printf("test B address = %p\n",testB);

  blockDim.x=16;*/


