import matplotlib.pyplot as plt 
import numpy as np
import sys
  
filepath = str(sys.argv[1])
left_motor = []
right_motor = []
image_center = []
error = []
duration = []
timestamp = []

with open(filepath) as fp:
   for line in fp:
       line = line.rstrip().split(",")
       line = [int(i) for i in line]
       left_motor.append(line[0])
       right_motor.append(line[1])
       image_center.append(line[2])
       error.append(line[3])
       duration.append(line[4])
       timestamp.append(line[5])


plt.subplot(2, 1, 1)
plt.plot(timestamp, left_motor, label = "Left motor speed") 
plt.plot(timestamp, right_motor, label = "Right motor speed")
plt.xlabel('time (micro s)') 
plt.ylabel('speed')
plt.title('Motors') 
plt.legend()

plt.subplot(2, 1, 2)
plt.plot(timestamp, image_center, label = "Set point") 
plt.plot(timestamp, np.add(image_center, error), label = "Output")
plt.xlabel('time (micro s)')
plt.ylabel('px')
plt.title('Image') 
plt.legend()
  
# function to show the plot 
plt.show() 

