import os

main_dir = "data"
splitted_dir = "splitted"
main_file = "test.txt"

def split(file, direct):
    f = open(main_dir + "/" + direct + "/" + file, "r")
    motor_left = open(splitted_dir + "/" + direct + "_ml.txt", "w")
    motor_right = open(splitted_dir + "/" + direct + "_mr.txt", "w")
    set_point = open(splitted_dir + "/" + direct + "_sp.txt", "w")
    error_and_sp = open(splitted_dir + "/" + direct + "_er&sp.txt", "w")
    exec_durr = open(splitted_dir + "/" + direct + "_ed.txt", "w")

    for line in f:
        line = line.rstrip('\r\n')
        line = line.split(',')
        motor_left.write(line[5]+' '+line[0]+'\n')
        motor_right.write(line[5]+' '+line[1]+'\n')
        set_point.write(line[5]+' '+line[2]+'\n')
        error_and_sp.write(line[5]+' '+str(int(line[3])+int(line[2]))+'\n')
        exec_durr.write(line[5]+' '+line[4]+'\n')

    f.close()
    motor_right.close()
    motor_left.close()
    set_point.close()
    error_and_sp.close()
    exec_durr.close()


if __name__== "__main__":
    for direct in os.listdir(main_dir):
        for file in os.listdir(main_dir + "/" + direct):
            if file == main_file:
                split(file, direct)
