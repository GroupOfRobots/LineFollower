import os

main_dir = "data"
splitted_dir = "splitted"

def split(file):
    f = open(main_dir + "/" + direct + "/" + file, "r")
    file_name = file[:-3]
    motor_left = open(splitted_dir + "/" + file_name + "_ml.txt", "w")
    motor_right = open(splitted_dir + "/" + file_name + "_mr.txt", "w")
    set_point = open(splitted_dir + "/" + file_name + "_sp.txt", "w")
    error = open(splitted_dir + "/" + file_name + "_er.txt", "w")
    exec_durr = open(splitted_dir + "/" + file_name + "_ed.txt", "w")

    for line in f:
        line = line.rstrip('\r\n')
        line = line.split(',')
        motor_left.write(line[5]+' '+line[0]+'\n')
        motor_right.write(line[5]+' '+line[1]+'\n')
        set_point.write(line[5]+' '+line[2]+'\n')
        error.write(line[5]+' '+line[3]+'\n')
        exec_durr.write(line[5]+' '+line[4]+'\n')

    f.close()
    motor_right.close()
    motor_left.close()
    set_point.close()
    error.close()
    exec_durr.close()


if __name__== "__main__":
    for direct in os.listdir(main_dir):
        for file in os.listdir(main_dir + "/" + direct):
            if file.endswith(".txt"):
                split(file)
                

