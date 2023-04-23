import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
from mpl_toolkits.mplot3d import Axes3D
from scipy import optimize
from scipy.spatial.transform import Rotation
import math
import cv2
import time

class SBM_120_UV:
    poly = None
    Debug = False
    position = None
    def __init__(self, pos):
        self.position = pos
        x = range(-90, 91, 10)
        y = [0.02, 0.05, 0.21, 0.43, 0.62, 0.78, 0.88, 0.95, 0.98, 1, 0.98, 0.95, 0.88, 0.78, 0.62, 0.43, 0.21, 0.05,
             0.02]
        p = np.polyfit(x, y, deg=6)
        self.poly = np.poly1d(p)
        if self.Debug:
            fig = plt.figure()
            xx = []
            yy = []
            for i in range(-90, 90, 1):
                xx.append(i)
                yy.append(self.poly(i))
            ax = fig.subplots()
            ax.plot(x, y, 'rx')
            ax.plot(xx, yy, 'b')

            plt.show()
    def shoot_ray(self, roll, pitch, z):
        angle = math.sqrt(roll*roll + pitch*pitch)
        ray = [0,0,0]
        if angle < 80:
            intensity = self.poly(angle)
            ray = np.array([0,0,1])
            rot = Rotation.from_euler('xyz', [roll, pitch, 0], degrees=True)
            ray = rot.apply(ray)
            distance_scale = z / ray[2]
            ray = ray*distance_scale
            ray[0] = ray[0] + self.position[0]
            ray[1] = ray[1] + self.position[1]
            ray[2] = intensity
            return [ray,True]
        return [ray,False]



z_start = 1
z_stop = 50
step_length = 10
total_number_of_iteration = z_stop-z_start

start_time = time.time()
iteration = 0
for z_distance in range(z_start,z_stop):
    number_of_leds = 1
    leds = []
    positions = [[95,95]]
    center_offset = [30,60]
    angular_offset = [0,30]
    angular_spacing = [60,30]
    j = 0
    for offset in center_offset:
        for i in range(0+angular_offset[j], 360+angular_offset[j], angular_spacing[j]):
            p = [offset*math.cos(i/180*math.pi), offset*math.sin(i/180*math.pi)]
            positions.append([positions[0][0]+p[0], positions[0][1]+p[1]])
            number_of_leds = number_of_leds+1
        j = j+1
    for i in range(0,number_of_leds):
        leds.append(SBM_120_UV(positions[i]))
    generate_image = True
    show_graph = False
    if generate_image:
        img_height = 190
        img_width = 190
        img = np.zeros((img_height,img_width,1), np.float32)
    # z_distance = 20
    x = []
    y = []
    z = []

    for roll in np.arange(-70,70,step_length):
        for pitch in np.arange(-70,70,step_length):
            for led in leds:
                ray, valid = led.shoot_ray(roll,pitch,z_distance)
                if not valid:
                    continue
                if generate_image:
                    px = [int(ray[0]),int(ray[1])]
                    if px[0]>=0 and px[0]<img_height and px[1]>=0 and px[1]<img_width:
                        img[px[0],px[1],0] = img[px[0],px[1],0]+ray[2]
                x.append(ray[0])
                y.append(ray[1])
                z.append(ray[2])

    if show_graph:
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        ax.scatter(x, y, z, 'b', s=10)
        plt.show()

    if generate_image:
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(img)
        for i in range(0,img_height):
            for j in range(0, img_width):
                img[i,j] = (img[i,j]-min_val)/(max_val-min_val)
        img = img*255
        img = img.astype(np.uint8)
        img_upscaled = cv2.resize(img, (1000,1000), interpolation = cv2.INTER_CUBIC)
        # img_upscaled = cv2.blur(img_upscaled,(9,9))
        cv2.circle(img_upscaled,(500,500),500,(255,255,255),3)
        cv2.putText(img_upscaled, 'z=%d'%z_distance, (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 255, 255), 2, cv2.LINE_AA)
        cv2.imshow("ray trace", img_upscaled)
        cv2.waitKey(100)
        cv2.imwrite('sweep/%d.png'%z_distance,img_upscaled)
        iteration = iteration+1
        print("%d/%d \ttime per iteration: %ds"%(iteration,total_number_of_iteration,(time.time()-start_time)/iteration))