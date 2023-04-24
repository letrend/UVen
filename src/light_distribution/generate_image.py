import cv2
import pickle
import numpy as np
import time
from multiprocessing import Pool

z_start = 6
z_stop = 50

def generate_image_func(z_distance):
    with open('/media/letrend/EE885F16885EDD21/sweep/%d.pickle' % z_distance, 'rb') as f:
        print("loading")
        rays = pickle.load(f)
        print("generating z=%d"%(z_distance))
        generate_image_t0 = time.time()
        img_height = 2000
        img_width = 2000
        img = np.zeros((img_height, img_width, 1), np.float32)
        invalid_rays = 0
        px = (np.array(rays[:, 0] * 10 + 300, dtype=int), np.array(rays[:, 1] * 10 + 300, dtype=int))
        j = 0
        for ray in rays:
            if ray[2]>0:
                if 0 <= px[0][j] < img_height and 0 <= px[1][j] < img_width:
                    img[px[0][j], px[1][j], 0] = img[px[0][j], px[1][j], 0] + ray[2]
            else:
                invalid_rays = invalid_rays+1
            j = j+1
            if j%1000000==0:
                print("%d/%d"%(j,len(rays)))

        print("%d invalid rays"%invalid_rays)

        img_scaled = img#cv2.resize(img, (1000, 1000), interpolation=cv2.INTER_CUBIC)
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(img_scaled)

        img_scaled = (img_scaled-min_val)/(max_val-min_val)*255
        # for i in range(0, img_height):
        #     for j in range(0, img_width):
        #         img[i, j] = (img[i, j] - min_val) / (max_val - min_val)
        # img = img * 255
        img_scaled = img_scaled.astype(np.uint8)
        cv2.circle(img_scaled, (int(img_scaled.shape[0]/2),int(img_scaled.shape[1]/2)), 700, (255, 255, 255), 3)
        cv2.putText(img_scaled, 'z=%d' % z_distance, (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 255, 255), 2,
                    cv2.LINE_AA)
        print("image generation took %ds"%(time.time()-generate_image_t0))
        cv2.imshow("ray trace", img_scaled)
        cv2.waitKey(100)
        cv2.imwrite('sweep/%d.png' % z_distance, img_scaled)
        return img_scaled

with Pool(processes=64) as pool:
    args = []
    for z_distance in range(z_start,z_stop):
        args.append([z_distance])
    async_results = [pool.apply_async(generate_image_func, args=arg) for arg in args ]
    results = [ar.get() for ar in async_results]
