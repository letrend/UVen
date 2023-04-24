import matplotlib.pyplot as plt
import cv2
import numpy as np
import math
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

__global__ void shoot_ray(float3 *rays, float *z, float3 *v, float* angle, float *x, float *y, size_t n)
{
  const int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if(idx<n){
      Quaternion q = Quaternion::describe_rotation(v[idx],angle[idx]);  
      float intensity = 0;
      for(int i=0;i<7;i++){
        intensity += coeffs[6-i]*powf(angle[idx],i);
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

angular_range_roll = 80
angular_range_pitch = 80
step_length = 0.5
z_distance = 5
roll = []
pitch = []
x = []
y = []

led_positions = [[95,95]]
# center_offset = [30,60]
# angular_offset = [0,30]
# angular_spacing = [60,30]
# j = 0
# for offset in center_offset:
#     for i in range(0+angular_offset[j], 360+angular_offset[j], angular_spacing[j]):
#         p = [offset*math.cos(i/180*math.pi), offset*math.sin(i/180*math.pi)]
#         led_positions.append([led_positions[0][0]+p[0], led_positions[0][1]+p[1]])

print("generating rays")
tmp_rot_axis = []
tmp_rot_angle = []
for i in np.arange(-angular_range_roll,angular_range_roll+step_length,step_length):
    for j in np.arange(-angular_range_pitch,angular_range_pitch+step_length,step_length):
        q = Quaternion(matrix=Rotation.from_euler('zyx', [0, i, j], degrees=True).as_matrix())
        if abs(q.angle)<1.3962634015955:
            tmp_rot_axis.append(q.axis)
            tmp_rot_angle.append(q.angle)
            # roll.append(i)
            # pitch.append(j)

rot_axis = []
rot_angle = []
for pos in led_positions:
    tmp_x = np.ones(len(tmp_rot_angle)) * pos[0]
    tmp_y = np.ones(len(tmp_rot_angle)) * pos[1]
    x.extend(tmp_x)
    y.extend(tmp_y)
    rot_axis.extend(tmp_rot_axis)
    rot_angle.extend(tmp_rot_angle)

# roll = np.array(roll,dtype=np.float32)
# pitch = np.array(pitch,dtype=np.float32)
rot_axis = np.array(rot_axis,dtype=np.float32)
rot_angle = np.array(rot_angle,dtype=np.float32)
x = np.array(x,dtype=np.float32)
y = np.array(y,dtype=np.float32)
print("running ray tracing")

for z_distance in range(5,50):
    z = np.ones(len(rot_axis)).astype(np.float32)*z_distance

    number_of_rays = len(rot_axis)

    rays = np.array([np.zeros_like(z),np.zeros_like(z),np.zeros_like(z)],dtype=np.float32)
    shoot_ray(
            drv.Out(rays), drv.In(z), drv.In(rot_axis), drv.In(rot_angle), drv.In(x), drv.In(y), drv.In(np.array(number_of_rays,dtype=int)),
            block=(16,1,1), grid=(int(number_of_rays/16),1))
    # A = np.array([z,roll,pitch],dtype=np.float32).transpose()
    rays = rays.reshape((number_of_rays,3))
    # print(rays==A)
    # print(rays)
    print(len(rays))

    img_height = 1900
    img_width = 1900
    img = np.zeros((img_height, img_width, 1), np.float32)
    invalid_rays = 0
    for ray in rays:
        if ray[2]>0:
            px = [int(ray[0]*10), int(ray[1]*10)]
            if px[0] >= 0 and px[0] < img_height and px[1] >= 0 and px[1] < img_width:
                img[px[0], px[1], 0] = img[px[0], px[1], 0] + ray[2]
        else:
            invalid_rays = invalid_rays+1

    print("%d invalid rays"%invalid_rays)

    img_scaled = img#cv2.resize(img, (1000, 1000), interpolation=cv2.INTER_CUBIC)
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(img_scaled)
    img_scaled = (img_scaled-min_val)/(max_val-min_val)*255
    # for i in range(0, img_height):
    #     for j in range(0, img_width):
    #         img[i, j] = (img[i, j] - min_val) / (max_val - min_val)
    # img = img * 255
    img_scaled = img_scaled.astype(np.uint8)
    cv2.circle(img_scaled, (int(img_scaled.shape[0]/2),int(img_scaled.shape[1]/2)), int(img_scaled.shape[0]/2), (255, 255, 255), 3)
    cv2.putText(img_scaled, 'z=%d' % z_distance, (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 255, 255), 2,
                cv2.LINE_AA)

    cv2.imshow("ray trace", img_scaled)
    cv2.waitKey(100)
    cv2.imwrite('sweep/%d.png' % z_distance, img_scaled)
# iteration = iteration + 1
# print("%d/%d \ttime per iteration: %ds" % (
# iteration, total_number_of_iteration, (time.time() - start_time) / iteration))


#
# # task function executed in a child worker process
# def shoot_ray(roll, pitch, z, position):
#     angle_abs = math.sqrt(roll * roll + pitch * pitch)
#     ray = [0, 0, 0]import numpy
#     if angle_abs < 80:
#         intensity = poly(angle_abs)
#         ray = np.array([0, 0, 1])
#         rot = Rotation.from_euler('xyz', [roll, pitch, 0], degrees=True)
#         ray = rot.apply(ray)
#         distance_scale = z / ray[2]
#         ray = ray * distance_scale
#         ray[0] = ray[0] + position[0]
#         ray[1] = ray[1] + position[1]
#         ray[2] = intensity
#         return [ray, True]
#     return [ray, False]
#
# step_length = 0.25
# z_distance = 39
#
# generate_image = True
# if generate_image:
#     img_height = 190
#     img_width = 190
#     img = np.zeros((img_height, img_width, 1), np.float32)
#
# led_positions = [[95,95]]
# center_offset = [30,60]
# angular_offset = [0,30]
# angular_spacing = [60,30]
# j = 0
# for offset in center_offset:
#     for i in range(0+angular_offset[j], 360+angular_offset[j], angular_spacing[j]):
#         p = [offset*math.cos(i/180*math.pi), offset*math.sin(i/180*math.pi)]
#         led_positions.append([led_positions[0][0]+p[0], led_positions[0][1]+p[1]])
#
# # protect the entry point
# if __name__ == '__main__':
#     # create the process pool
#     with Pool(processes=256) as pool:
#         # issue multiple tasks each with multiple arguments
#         roll = []
#         pitch = []
#         args = []
#         for pos in led_positions:
#             for i in np.arange(-70, 70, step_length):
#                 for j in np.arange(-70, 70, step_length):
#                     args.append([i,j,z_distance,pos])
#         async_results = [pool.apply_async(shoot_ray, args=arg) for arg in args]
#         # retrieve the return value results
#         results = [ar.get() for ar in async_results]
#         print("%d rays"%len(results))
#         if generate_image:
#             if generate_image:
#                 for ray in results:
#                     if ray[1]:
#                         px = [int(ray[0][0]), int(ray[0][1])]
#                         if px[0] >= 0 and px[0] < img_height and px[1] >= 0 and px[1] < img_width:
#                             img[px[0], px[1], 0] = img[px[0], px[1], 0] + ray[0][2]
#             min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(img)
#             for i in range(0, img_height):
#                 for j in range(0, img_width):
#                     img[i, j] = (img[i, j] - min_val) / (max_val - min_val)
#             img = img * 255
#             img = img.astype(np.uint8)
#             img_upscaled = cv2.resize(img, (1000, 1000), interpolation=cv2.INTER_CUBIC)
#             # img_upscaled = cv2.blur(img_upscaled,(9,9))
#             cv2.circle(img_upscaled, (500, 500), 500, (255, 255, 255), 3)
#             cv2.putText(img_upscaled, 'z=%d' % z_distance, (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 255, 255), 2,
#                         cv2.LINE_AA)
#             cv2.imshow("ray trace", img_upscaled)
#             cv2.waitKey(0)
#             # cv2.imwrite('sweep/%d.png' % z_distance, img_upscaled)
#             # iteration = iteration + 1
#             # print("%d/%d \ttime per iteration: %ds" % (
#             # iteration, total_number_of_iteration, (time.time() - start_time) / iteration))