import matplotlib.pyplot as plt
import cv2
import numpy as np
import math
from scipy.spatial.transform import Rotation
import pycuda.driver as cuda
import pycuda.autoinit
from pycuda.compiler import SourceModule
from pycuda import gpuarray, tools

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

__global__ void shoot_ray(float3 *rays, float *z, float *pitch, float *roll, float *x, float *y, size_t n)
{
  const int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if(idx<n){
      float angle = sqrtf(pitch[idx]*pitch[idx]+roll[idx]*roll[idx]);
      if(abs(angle)<1.3962634015955){
          float intensity = 0;
          for(int i=0;i<7;i++){
            intensity += coeffs[6-i]*pow(angle,i);
          }
          float rot[3][3];
          rot[0][0] = cosf(pitch[idx])*cosf(roll[idx]);
          rot[1][0] = cosf(pitch[idx])*sinf(roll[idx]);
          rot[2][0] = -sin(pitch[idx]);
          rot[0][1] = -sinf(roll[idx]);
          rot[1][1] = cosf(roll[idx]);
          rot[2][1] = 0;
          rot[0][2] = sinf(pitch[idx])*cosf(roll[idx]);
          rot[1][2] = sinf(pitch[idx])*sinf(roll[idx]);
          rot[2][2] = cosf(pitch[idx]);
          float dir[3], dir_new[3];
          dir[0] = 0; dir[1] = 0; dir[2] = 1;
          dir_new[0] = 0; dir_new[1] = 0; dir_new[2] = 0;
          
          for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                dir_new[i]+=rot[i][j]*dir[j];
            }
          }
          
          float distance_scale = z[idx] / dir_new[2];
          
          rays[idx].x = dir_new[0]*distance_scale+x[idx];
          rays[idx].y = dir_new[1]*distance_scale+y[idx];
          rays[idx].z = intensity;
      }else{
          rays[idx].x = 0;
          rays[idx].y = 0;
          rays[idx].z = 0; 
      }
  }
}
""")

shoot_ray = mod.get_function("shoot_ray")
coeffs_cuda = mod.get_global("coeffs")
drv.memcpy_htod(coeffs_cuda[0],coeffs)

step_length = 0.1
z_distance = 1
roll = []
pitch = []
x = []
y = []

led_positions = [[95,95]]
center_offset = [30,60]
angular_offset = [0,30]
angular_spacing = [60,30]
j = 0
for offset in center_offset:
    for i in range(0+angular_offset[j], 360+angular_offset[j], angular_spacing[j]):
        p = [offset*math.cos(i/180*math.pi), offset*math.sin(i/180*math.pi)]
        led_positions.append([led_positions[0][0]+p[0], led_positions[0][1]+p[1]])

for pos in led_positions:
    for i in np.arange(-80*np.pi/180,80*np.pi/180,step_length*np.pi/180):
        for j in np.arange(-80*np.pi/180,80*np.pi/180,step_length*np.pi/180):
            roll.append(i)
            pitch.append(j)
            x.append(pos[0])
            y.append(pos[1])

roll = np.array(roll,dtype=np.float32)
pitch = np.array(pitch,dtype=np.float32)
x = np.array(x,dtype=np.float32)
y = np.array(y,dtype=np.float32)
z = np.ones(len(roll)).astype(np.float32)*z_distance

number_of_rays = len(roll)

rays = np.array([np.zeros_like(z),np.zeros_like(z),np.zeros_like(z)],dtype=np.float32)
shoot_ray(
        drv.Out(rays), drv.In(z), drv.In(roll), drv.In(pitch), drv.In(x), drv.In(y), drv.In(np.array(number_of_rays,dtype=int)),
        block=(16,1,1), grid=(int(number_of_rays/16),1))
A = np.array([z,roll,pitch],dtype=np.float32).transpose()
rays = rays.reshape((len(roll),3))
print(rays==A)
print(rays)
print(len(rays))

img_height = 190
img_width = 190
img = np.zeros((img_height, img_width, 1), np.float32)
invalid_rays = 0
for ray in rays:
    if ray[2]>0:
        px = [int(ray[0]), int(ray[1])]
        if px[0] >= 0 and px[0] < img_height and px[1] >= 0 and px[1] < img_width:
            img[px[0], px[1], 0] = img[px[0], px[1], 0] + ray[2]
    else:
        invalid_rays = invalid_rays+1
print("%d invalid rays"%invalid_rays)
min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(img)
for i in range(0, img_height):
    for j in range(0, img_width):
        img[i, j] = (img[i, j] - min_val) / (max_val - min_val)
img = img * 255
img = img.astype(np.uint8)
img_upscaled = cv2.resize(img, (1000, 1000), interpolation=cv2.INTER_CUBIC)
# img_upscaled = cv2.blur(img_upscaled,(9,9))
cv2.circle(img_upscaled, (500, 500), 500, (255, 255, 255), 3)
cv2.putText(img_upscaled, 'z=%d' % z_distance, (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 255, 255), 2,
            cv2.LINE_AA)
cv2.imshow("ray trace", img_upscaled)
cv2.waitKey(0)
# cv2.imwrite('sweep/%d.png' % z_distance, img_upscaled)
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