import sys
import time as time
import numpy as np
import scipy as sp
import pylab as pl
from sklearn.feature_extraction.image import grid_to_graph
from sklearn.cluster import Ward
from skimage.filter import denoise_tv_chambolle
import scipy.ndimage as ndI
from PIL import Image
import matplotlib.pyplot as plt
from skimage.segmentation import felzenszwalb, slic, quickshift
from skimage.segmentation import mark_boundaries
import matplotlib.cm as cm
#from cheese_opencv import CheeseOpenCV
#from skimage import filter


def get_bounds(image_result_matrix, index_1, index_2):
            
    bounds = []
    
    if index_1 is not 0:
        bounds.append(image_result_matrix[index_1 - 1][index_2])

    if index_2 is not 0:
        bounds.append(image_result_matrix[index_1][index_2 - 1])
    
    return bounds
        
    
def create_segmented_cluster_std_estimate(first_std_matrix, second_std_matrix, transitional_matrix):
    first_image_cluster_segments_std = {}
    first_image_cluster_ocurrence = {}
    
    for index_1 in range(len(first_std_matrix)):
        for index_2 in range(len(first_std_matrix[index_1])):
            deviation_value = first_std_matrix[index_1][index_2]
            
            cluster_tuple = transitional_matrix[index_1][index_2]

            if cluster_tuple in first_image_cluster_segments_std:
                first_image_cluster_segments_std[cluster_tuple] = first_image_cluster_segments_std[cluster_tuple] + deviation_value
                first_image_cluster_ocurrence[cluster_tuple] = first_image_cluster_ocurrence[cluster_tuple] + 1
            else:
                first_image_cluster_segments_std[cluster_tuple] = deviation_value
                first_image_cluster_ocurrence[cluster_tuple] = 1

    for key in first_image_cluster_segments_std:
        first_image_cluster_segments_std[key] = first_image_cluster_segments_std[key] / first_image_cluster_ocurrence[key]
    
    second_image_cluster_segments_std = {}
    second_image_cluster_ocurrence = {}
    
    for index_1 in range(len(second_std_matrix)):
        for index_2 in range(len(second_std_matrix[index_1])):
            deviation_value = second_std_matrix[index_1][index_2]

            cluster_tuple = transitional_matrix[index_1][index_2]

            if cluster_tuple in second_image_cluster_segments_std:
                second_image_cluster_segments_std[cluster_tuple] = second_image_cluster_segments_std[cluster_tuple] + deviation_value
                second_image_cluster_ocurrence[cluster_tuple] = second_image_cluster_ocurrence[cluster_tuple] + 1
            else:
                second_image_cluster_segments_std[cluster_tuple] = deviation_value
                second_image_cluster_ocurrence[cluster_tuple] = 1

    for key in second_image_cluster_segments_std:
        second_image_cluster_segments_std[key] = second_image_cluster_segments_std[key] / second_image_cluster_ocurrence[key]
    
    return [first_image_cluster_segments_std, second_image_cluster_segments_std]


def merge_matrixes(first_label_matrix, second_label_matrix):
    final_matrix = np.empty((len(first_label_matrix), len(first_label_matrix[0])), dtype=object)
    
    for index_1 in range(len(first_label_matrix)):
        for index_2 in range(len(first_label_matrix[index_1])):
            final_matrix[index_1][index_2] = str(str(first_label_matrix[index_1][index_2]) + "-"+ str(second_label_matrix[index_1][index_2]))
            
    return final_matrix


def image_std(image):
    image_width, image_height = pil_image.size
    first_image = np.asarray(pil_image)
    image = pl.mean(first_image, 2)
    
    box_height = box_width = 3
    
    new_image = np.zeros(shape=(len(image),len(image[0])))
    
    for index_y in range(int(image_height/box_height)):
        for index_x in range(int(image_width/box_width)):
            
            points_array = [image[index_y * box_height, index_x * box_width], image[index_y * box_height,index_x * box_width + 1] , image[index_y * box_height + 1, index_x * box_width] , image[index_y * box_height + 1, index_x * box_width + 1], image[index_y * box_height + 2, index_x * box_width + 1], image[index_y * box_height + 1, index_x * box_width + 2], image[index_y * box_height, index_x * box_width + 2], image[index_y * box_height + 2, index_x * box_width], image[index_y * box_height + 2, index_x * box_width + 2]]
            points_std = np.std(points_array)

            new_image[index_y * box_height, index_x * box_width] = points_std
            new_image[index_y * box_height, index_x * box_width + 1] = points_std
            new_image[index_y * box_height + 1, index_x * box_width] = points_std
            new_image[index_y * box_height + 1, index_x * box_width + 1] = points_std
            new_image[index_y * box_height + 1, index_x * box_width + 2] = points_std
            new_image[index_y * box_height + 2, index_x * box_width + 1] = points_std
            new_image[index_y * box_height + 2, index_x * box_width + 2] = points_std
            new_image[index_y * box_height + 2, index_x * box_width + 0] = points_std
            new_image[index_y * box_height + 0, index_x * box_width + 2] = points_std
            # points_array1 = []
            # for box_height_index in range(box_height):
            #    for box_width_index in range(box_width):
            #        points_array1.append(image[index_y * box_height + box_height_index, index_x * box_width_index])
            # 
            # points_std = np.std(points_array1)
            # 
            # for box_height_index in range(box_height):
            #    for box_width_index in range(box_width):
            #        new_image1[index_y * box_height + box_height_index, index_x * box_width + box_width_index] = points_std
            
            
    fig = pl.figure(figsize=(5, 5))
    pl.imshow(new_image)
    pl.xticks(())
    pl.yticks(())
    fig.show()
    
    return new_image


###############################################################################
## BEGINNING
###############################################################################
st = time.time()

# Quickshift clustering alg. parameters
kernel_size = 9
max_dist = 14
ratio = 0.15

#anti_shake = CheeseOpenCV()
#anti_shake.openCV(str(sys.argv[1]), str(sys.argv[2]))

#first_image_name = "antiShake_0.jpg"
#second_image_name = "antiShake_1.jpg"

first_image_name = str(sys.argv[1])
second_image_name = str(sys.argv[2])

#scale = 0.25 # from 0 to 1
scale = float(sys.argv[3])

first_slic_ratio = int(sys.argv[4])
first_n_segments = int(sys.argv[5])
first_sigma = int(sys.argv[6])
first_max_iter = 10

second_slic_ratio = int(sys.argv[7])
second_n_segments = int(sys.argv[8])
second_sigma = int(sys.argv[9])
second_max_iter = 10

###############################################################################
## FIRST IMAGE
###############################################################################
pil_image = Image.open(first_image_name).convert('RGB')
width, height = pil_image.size

if scale != 1:
    pil_image = pil_image.resize((int(scale*width), int(scale*height)), Image.ANTIALIAS)

first_std_image = image_std(pil_image)

first_image = np.asarray(pil_image)
first_image_final = np.copy(first_image)

# Compute clustering
print("Compute structured hierarchical clustering...")

#first_label = felzenszwalb(pil_image, scale=100, sigma=0.5, min_size=50)
#first_label = slic(first_image, ratio=first_slic_ratio, n_segments=first_n_segments, sigma=first_sigma, max_iter=first_max_iter)
first_label = np.zeros(shape=(len(first_image),len(first_image[0])))
#first_image_edges = filter.canny(first_image, sigma=5)
#first_label = quickshift(pil_image, kernel_size=kernel_size, max_dist=max_dist, ratio=ratio)

fig, ax = plt.subplots(1, 3)
fig.set_size_inches(8, 3, forward=True)
plt.subplots_adjust(0.05, 0.05, 0.95, 0.95, 0.05, 0.05)
ax[0].imshow(mark_boundaries(first_image_final, first_label))
ax[0].set_title("1st Image")

first_image = pl.mean(first_image,2)

first_label = np.reshape(first_label, first_image.shape)

###############################################################################
#SECOND IMAGE
###############################################################################
pil_image = Image.open(second_image_name)

if scale != 1:
    pil_image = pil_image.resize((int(scale*width), int(scale*height)), Image.ANTIALIAS)

pil_image = pil_image.convert('RGB')
second_image = np.asarray(pil_image)

second_std_image = image_std(pil_image)

second_image_final = np.copy(second_image)

print("Compute structured hierarchical clustering...")

#second_label = felzenszwalb(pil_image, scale=100, sigma=0.5, min_size=50)
second_label = slic(second_image, ratio=second_slic_ratio, n_segments=second_n_segments, sigma=second_sigma, max_iter=second_max_iter)
#second_image_edges = filter.canny(second_image, sigma=4)
#second_label = quickshift(pil_image, kernel_size=kernel_size, max_dist=max_dist, ratio=ratio)

ax[1].imshow(mark_boundaries(second_image_final, second_label))
ax[1].set_title("2nd Image")

second_image = pl.mean(second_image,2)

second_label = np.reshape(second_label, second_image.shape)

###############################################################################
# POST PROCESSING
###############################################################################
transitional_matrix = merge_matrixes(first_label, second_label)

segmented_stds = create_segmented_cluster_std_estimate(first_std_image, second_std_image, transitional_matrix)
first_image_cluster_segments_std = segmented_stds[0]
second_image_cluster_segments_std = segmented_stds[1]

print "LIST OF STD() PER SEGMENT:"

dead_cluster_segments_list = []

for key in first_image_cluster_segments_std:
    print key
    print first_image_cluster_segments_std[key]
    print second_image_cluster_segments_std[key]
    print " "
    
    #if first_image_cluster_segments_std[key] < 1.278 and second_image_cluster_segments_std[key] < 1.278  :
    #    dead_cluster_segments_list.append(key)
    
#print dead_cluster_segments_list

dead_cluster_binary_matrix = np.zeros(shape=(len(transitional_matrix),len(transitional_matrix[0])))
index_1_dead_array = []
index_2_dead_array = []

for index_1 in range(len(transitional_matrix)):
    for index_2 in range(len(transitional_matrix[index_1])):
        
        if transitional_matrix[index_1][index_2] in dead_cluster_segments_list:
            dead_cluster_binary_matrix[index_1][index_2] = 1
            index_1_dead_array.append(index_1)
            index_2_dead_array.append(index_2)
            

if len(index_1_dead_array) > 0:
    ## Create dead clusters neighbor box
    dead_box_1_max = max(index_1_dead_array)
    dead_box_1_min = min(index_1_dead_array)
    dead_box_2_max = max(index_2_dead_array)
    dead_box_2_min = min(index_2_dead_array)


    ## Calculate std() for dead cluster neighbor box in each image
    first_image_dead_cluster_std = 0.0
    second_image_dead_cluster_std = 0.0

    for index_1 in range(len(first_std_image)):
    
        if index_1 < dead_box_1_max and index_1 > dead_box_1_min:
        
            for index_2 in range(len(first_std_image[index_1])):
            
                if index_2 < dead_box_2_max and index_2 > dead_box_2_min:
                    first_image_dead_cluster_std = first_image_dead_cluster_std + first_std_image[index_1][index_2]
                    second_image_dead_cluster_std = second_image_dead_cluster_std + second_std_image[index_1][index_2]


    if first_image_dead_cluster_std > second_image_dead_cluster_std:
        dead_box_owner = 1
    else:
        dead_box_owner = 2

    fig = pl.figure(figsize=(5, 5))
    pl.imshow(dead_cluster_binary_matrix, cmap = cm.hot)
    pl.xticks(())
    pl.yticks(())
    fig.show()


## CREATE 2D REPRESENTATION OF THE STD() ON BOTH IMAGE'S PIXELS DIVIDED BY THE INTERSECTION OF THE CLUSTERS MATRIXES
first_image_cluster_segments_std_array = np.zeros(shape=(len(transitional_matrix),len(transitional_matrix[0])))
second_image_cluster_segments_std_array = np.zeros(shape=(len(transitional_matrix),len(transitional_matrix[0])))
for index_1 in range(len(transitional_matrix)):
    for index_2 in range(len(transitional_matrix[index_1])):
        transitional_tuple = transitional_matrix[index_1][index_2]
        first_image_cluster_segments_std_array[index_1][index_2] = first_image_cluster_segments_std[transitional_tuple]
        second_image_cluster_segments_std_array[index_1][index_2] = second_image_cluster_segments_std[transitional_tuple]


## PRINT STD() GRAPHS
fig = pl.figure(figsize=(5, 5))
pl.imshow(first_image_cluster_segments_std_array, cmap = cm.Greys_r)
pl.xticks(())
pl.yticks(())
fig.show()

fig = pl.figure(figsize=(5, 5))
pl.imshow(second_image_cluster_segments_std_array, cmap = cm.Greys_r)
pl.xticks(())
pl.yticks(())
fig.show()


#print label
print("Elapsed time: ", time.time() - st)
print("Number of pixels: ", second_label.size)
print("Number of clusters: " +str(np.unique(second_label).size) + "  " +  str(np.unique(first_label).size))

###############################################################################
# FINAL IMAGE
###############################################################################
final_image = np.copy(first_image_final)
final_image_matrix = np.zeros(shape=(len(first_image_final),len(first_image_final[0])))

for index_1 in range(len(final_image)):
    for index_2 in range(len(final_image[index_1])):
        
        if transitional_matrix[index_1][index_2] in dead_cluster_segments_list:
            if dead_box_owner is 1:
                final_image[index_1][index_2] = first_image_final[index_1][index_2]
            else:
                final_image[index_1][index_2] = second_image_final[index_1][index_2]
                
        else:
            first_image_delta = first_image_cluster_segments_std_array[index_1][index_2]
            second_image_delta = second_image_cluster_segments_std_array[index_1][index_2]
        
            
            #Compare modules
            if first_image_delta > second_image_delta:
                final_image[index_1][index_2] = [0, 0, 0]
                final_image_matrix[index_1][index_2] = 0
            else:
                final_image[index_1][index_2] = [255, 255, 255]
                final_image_matrix[index_1][index_2] = 1                
                

###############################################################################
# GEOMETRY ANALISYS
###############################################################################

first_image_result_matrix = np.zeros(shape=(len(first_image_final),len(first_image_final[0])), dtype=np.int)
second_image_result_matrix = np.zeros(shape=(len(first_image_final),len(first_image_final[0])), dtype=np.int)

geometry_image_cluster_index = int(1)

for index_1 in range(len(final_image_matrix)):
    for index_2 in range(len(final_image_matrix[index_1])):
        
        if int(final_image_matrix[index_1][index_2]) is 0:
            is_first = True
            bounds = get_bounds(first_image_result_matrix, index_1, index_2)
        else:
            is_first = False
            bounds = get_bounds(second_image_result_matrix, index_1, index_2)
        
        if len(bounds) is 0:
            #####CREATE

            if is_first:
                first_image_result_matrix[index_1][index_2] = geometry_image_cluster_index
            else:
                second_image_result_matrix[index_1][index_2] = geometry_image_cluster_index
            geometry_image_cluster_index = int(geometry_image_cluster_index + 1)
            
        elif len(bounds) is 1:
            
            if int(bounds[0]) is 0:
                #####CREATE

                if is_first:
                    first_image_result_matrix[index_1][index_2] = geometry_image_cluster_index
                else:
                    second_image_result_matrix[index_1][index_2] = geometry_image_cluster_index
                geometry_image_cluster_index = int(geometry_image_cluster_index + 1)
                
            else:
                # ASSIGN
                if is_first:
                    first_image_result_matrix[index_1][index_2] = bounds[0]
                else:
                    second_image_result_matrix[index_1][index_2] = bounds[0]
            
        elif len(bounds) is 2:

            if int(bounds[0]) is 0 and int(bounds[1]) is 0:
                ####CREATE

                if is_first:
                    first_image_result_matrix[index_1][index_2] = geometry_image_cluster_index
                else:
                    second_image_result_matrix[index_1][index_2] = geometry_image_cluster_index
                geometry_image_cluster_index = int(geometry_image_cluster_index + 1)
                
            elif int(bounds[0]) is int(bounds[1]) or int(bounds[0]) is 0 or int(bounds[1]) is 0:
                # ASSIGN
                if is_first:
                    if int(bounds[0]) is 0:
                        first_image_result_matrix[index_1][index_2] = bounds[1]
                    else:
                        first_image_result_matrix[index_1][index_2] = bounds[0]
                else:
                    if int(bounds[0]) is 0:
                        second_image_result_matrix[index_1][index_2] = bounds[1]
                    else:
                        second_image_result_matrix[index_1][index_2] = bounds[0]
            else:
                #MERGE AND ASSIGN
                if is_first:
                    for index_final_1 in range(len(first_image_result_matrix)):
                        for index_final_2 in range(len(first_image_result_matrix[index_final_1])):
                            if int(first_image_result_matrix[index_final_1][index_final_2]) is int(bounds[1]):
                                first_image_result_matrix[index_final_1][index_final_2] = int(bounds[0])
                            
                            
                    first_image_result_matrix[index_1][index_2] = bounds[0]
                else:
                    for index_final_1 in range(len(second_image_result_matrix)):
                        for index_final_2 in range(len(second_image_result_matrix[index_final_1])):
                            if int(second_image_result_matrix[index_final_1][index_final_2]) is int(bounds[1]):
                                second_image_result_matrix[index_final_1][index_final_2] = int(bounds[0])
                                
                    second_image_result_matrix[index_1][index_2] = bounds[0]

## PLOT GEOMETRIES
fig = pl.figure(figsize=(5, 5))
pl.imshow(first_image_result_matrix)
pl.xticks(())
pl.yticks(())
fig.show()
fig = pl.figure(figsize=(5, 5))
pl.imshow(second_image_result_matrix)
pl.xticks(())
pl.yticks(())
fig.show()

## CALCULATE THE NUMBER OF POINTS PER CLUSTER SEGMENT GEOMETRY
first_image_unique_geometries = np.unique(first_image_result_matrix)
second_image_unique_geometries = np.unique(second_image_result_matrix)

first_image_geometry_count = {}
second_image_geometry_count = {}

for geometry_index in first_image_unique_geometries:
    if int(geometry_index) is not 0:
        geometry_count = 0
        for index_1 in range(len(first_image_result_matrix)):
            for index_2 in range(len(first_image_result_matrix[index_1])):        
                if int(first_image_result_matrix[index_1][index_2]) is int(geometry_index):
                    geometry_count = geometry_count + 1
        
        first_image_geometry_count[geometry_index] = geometry_count
        
for geometry_index in second_image_unique_geometries:
    if int(geometry_index) is not 0:
        geometry_count = 0
        for index_1 in range(len(second_image_result_matrix)):
            for index_2 in range(len(second_image_result_matrix[index_1])):        
                if int(second_image_result_matrix[index_1][index_2]) is int(geometry_index):
                    geometry_count = geometry_count + 1
        
        second_image_geometry_count[geometry_index] = geometry_count
        
        
### REMOVE SEGMENT GEOMETRIES WITH FEW POINTS

for index_1 in range(len(final_image_matrix)):
    for index_2 in range(len(final_image_matrix[index_1])):
        
        if int(final_image_matrix[index_1][index_2]) is 0:
            #first_image
            if first_image_geometry_count[first_image_result_matrix[index_1][index_2]] < 1000:
                final_image_matrix[index_1][index_2] = 1
        else:
            #second_image
            if second_image_geometry_count[second_image_result_matrix[index_1][index_2]] < 1000:
                final_image_matrix[index_1][index_2] = 0

## PLOT FINAL MATRIX
fig = pl.figure(figsize=(5, 5))
pl.imshow(final_image_matrix)
pl.xticks(())
pl.yticks(())
fig.show()

### SAVE AND PLOT IMAGES
print first_image_geometry_count

sp.misc.imsave("output/" + str(width) + "_" + str(height) + "_" + str(scale) + "_" +  str(second_slic_ratio) + "_" + str(second_n_segments) + "_" + str(second_sigma) + '.png', final_image)  

# Adds 3rd image and print big graph:
ax[2].imshow(final_image)
ax[2].set_title("Result Image")
for a in ax:
    a.set_xticks(())
    a.set_yticks(())
plt.show()