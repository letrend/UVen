import matplotlib.pyplot as plt
import cv2
import numpy as np
import math
import time
import pickle
from scipy.spatial.transform import Rotation
import pycuda.driver as cuda
import pycuda.autoinit
from pycuda.compiler import SourceModule
from pycuda import gpuarray, tools
from pyquaternion import Quaternion
import pycuda.autoinit
import pycuda.driver as drv
import numpy as np
from pycuda.compiler import SourceModule

x = range(-90, 91, 10)
y = [0.02, 0.05, 0.21, 0.43, 0.62, 0.78, 0.88, 0.95, 0.98, 1, 0.98, 0.95, 0.88, 0.78, 0.62, 0.43, 0.21, 0.05,
     0.02]
p = np.polyfit(x, y, deg=6)
poly = np.poly1d(p)

val = 1
coeffs = np.array(poly.coeffs,dtype=np.float32)
fval = 0
for i in range(0,len(coeffs)):
    fval = fval + coeffs[6-i]*np.power(val,i)
print(fval)
print(poly(0))



mod = SourceModule("""
__constant__ float coeffs[7];

// Quaternion helper class describing rotations
// this allows for a nice description and execution of rotations in 3D space.
typedef struct _Quaternion
{
    // rotation around a given axis (given sine and cosine of HALF the rotation angle)
    static __device__ __forceinline__ struct _Quaternion describe_rotation(const float3 v, const float sina_2, const float cosa_2)
    {
        struct _Quaternion result;
        result.q = make_float4(cosa_2, sina_2*v.x, sina_2*v.y, sina_2*v.z);
        return result;
    }

    // rotation around a given axis (angle without range restriction)
    static __device__ __forceinline__ struct _Quaternion describe_rotation(const float3 v, const float angle)
    {
        float sina_2 = sinf(angle/2);
        float cosa_2 = cosf(angle/2);
        struct _Quaternion result;
        result.q = make_float4(cosa_2, sina_2*v.x, sina_2*v.y, sina_2*v.z);
        return result;
    }

    // rotate a point v in 3D space around the origin using this quaternion
    // see EN Wikipedia on Quaternions and spatial rotation
    __device__ __forceinline__ float3 rotate(const float3 v) const
    {
        float t2 =   q.x*q.y;
        float t3 =   q.x*q.z;
        float t4 =   q.x*q.w;
        float t5 =  -q.y*q.y;
        float t6 =   q.y*q.z;
        float t7 =   q.y*q.w;
        float t8 =  -q.z*q.z;
        float t9 =   q.z*q.w;
        float t10 = -q.w*q.w;
        return make_float3(
            2.0f*( (t8 + t10)*v.x + (t6 -  t4)*v.y + (t3 + t7)*v.z ) + v.x,
            2.0f*( (t4 +  t6)*v.x + (t5 + t10)*v.y + (t9 - t2)*v.z ) + v.y,
            2.0f*( (t7 -  t3)*v.x + (t2 +  t9)*v.y + (t5 + t8)*v.z ) + v.z
        );
    }

    // rotate a point v in 3D space around a given point p using this quaternion
    __device__ __forceinline__ float3 rotate_around_p(const float3 v, const float3 p)
    {
        float3 tmp;
        tmp.x = v.x-p.x;
        tmp.y = v.y-p.y;
        tmp.z = v.z-p.z;
        float3 tmp2 = rotate(tmp);
        tmp2.x += p.x;
        tmp2.z += p.y;
        tmp2.z += p.z;
        return tmp2;
    }

    // 1,i,j,k
    float4 q;
} Quaternion;

__global__ void shoot_ray(float3 *rays, float *z, float3 *v, float* angle, float *x, float *y, float *angle_abs, size_t n)
{
  const int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if(idx<n){
      Quaternion q = Quaternion::describe_rotation(v[idx],angle[idx]);  
      float intensity = 0;
      for(int i=0;i<7;i++){
        intensity += coeffs[6-i]*powf(angle_abs[idx],i);
      }
      
      float3 dir_new;
      dir_new.x = 0;
      dir_new.y = 0;
      dir_new.z = 1;
      
      dir_new = q.rotate(dir_new);
      
      float distance_scale = z[idx] / dir_new.z;
      
      rays[idx].x = dir_new.x*distance_scale+x[idx];
      rays[idx].y = dir_new.y*distance_scale+y[idx];
      rays[idx].z = intensity;
  }
}
""")

shoot_ray = mod.get_function("shoot_ray")
coeffs_cuda = mod.get_global("coeffs")
drv.memcpy_htod(coeffs_cuda[0],coeffs)

angular_range_roll = 70
angular_range_pitch = 70
step_length = 0.05
z_start = 5
z_stop = 50
roll = []
pitch = []
x = []
y = []

led_positions = [[70,70]]
center_offset = [28,56,50]
angular_offset = [0,0,36]
angular_spacing = [72,72,72]
j = 0

for offset in center_offset:
    for i in range(0+angular_offset[j], 360+angular_offset[j], angular_spacing[j]):
        p = [offset*math.cos(i/180.0*math.pi), offset*math.sin(i/180.0*math.pi)]
        led_positions.append([led_positions[0][0]+p[0], led_positions[0][1]+p[1]])
    j = j+1

print("generating rays")
tmp_rot_axis = []
tmp_rot_angle = []
tmp_angle_abs = []
iterations = int((2*angular_range_pitch+step_length)/step_length)
iter = 0
for i in np.arange(-angular_range_roll,angular_range_roll+step_length,step_length):
    if iter%10==0:
        print("%d/%d"%(iter,iterations))
    for j in np.arange(-angular_range_pitch,angular_range_pitch+step_length,step_length):
        angle = np.sqrt(i ** 2 + j ** 2)
        q = Quaternion(matrix=Rotation.from_euler('zyx', [0, i, j], degrees=True).as_matrix())
        if angle<80:
            tmp_rot_axis.append(q.axis)
            tmp_rot_angle.append(q.angle)
            tmp_angle_abs.append(angle)
            # roll.append(i)
            # pitch.append(j)
    iter = iter+1

print("generating leds")
rot_axis = []
rot_angle = []
angle_abs = []
j = 0
for pos in led_positions:
    print("%d/%d"%(j,len(led_positions)))
    tmp_x = np.ones(len(tmp_rot_angle)) * pos[0]
    tmp_y = np.ones(len(tmp_rot_angle)) * pos[1]
    x.extend(tmp_x)
    y.extend(tmp_y)
    rot_axis.extend(tmp_rot_axis)
    rot_angle.extend(tmp_rot_angle)
    angle_abs.extend(tmp_angle_abs)
    j = j+1

number_of_rays = len(rot_axis)
# roll = np.array(roll,dtype=np.float32)
# pitch = np.array(pitch,dtype=np.float32)
rot_axis = np.array(rot_axis,dtype=np.float32)
rot_axis = rot_axis.reshape((3,number_of_rays))
rot_angle = np.array(rot_angle,dtype=np.float32)
angle_abs = np.array(angle_abs,dtype=np.float32)
x = np.array(x,dtype=np.float32)
y = np.array(y,dtype=np.float32)
print("running ray tracing")

first_iteration = True
min_val = 0
max_val = 0
start_time = time.time()
for z_distance in range(z_start,z_stop):
    z = np.ones(number_of_rays).astype(np.float32)*z_distance

    rays = np.array([np.zeros_like(z),np.zeros_like(z),np.zeros_like(z)],dtype=np.float32)
    t0_ray_trace = time.time()
    shoot_ray(
            drv.Out(rays), drv.In(z), drv.In(rot_axis), drv.In(rot_angle), drv.In(x), drv.In(y), drv.In(angle_abs), drv.In(np.array(number_of_rays,dtype=int)),
            block=(16,1,1), grid=(int(number_of_rays/16),1))
    # A = np.array([z,roll,pitch],dtype=np.float32).transpose()
    rays = rays.reshape((number_of_rays,3))
    # print(rays==A)
    # print(rays)
    print("ray tracing took %ds for %d rays"%(time.time()-t0_ray_trace,len(rays)))

    with open('/media/letrend/EE885F16885EDD21/sweep/%d.pickle'%z_distance, 'wb') as f:
        pickle.dump(rays, f)


