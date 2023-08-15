import serial
import rospy
from std_msgs.msg import Int32
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=0.1)  # open serial port
rospy.init_node("temp_publisher")

pub = rospy.Publisher('temp_external', Int32, queue_size=1)


while not rospy.is_shutdown():
    ser.write(b'tempshow\n')     # write a string
    a = ser.readline()
    b = ser.readline()
    c = ser.readline()
    if c[0]==43:
        pub.publish(Int32(int(c[1:-2])))
        # print(c)
    rospy.Rate(1).sleep()
ser.close()             # close port