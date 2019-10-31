import os

def split(file):
    pass


if __name__== "__main__":
    for direct in os.listdir("data"):
        for file in os.listdir("data/" + direct):
            if file.endswith(".txt"):
                f = open("data/" + direct + "/" + file, "r")
                split(f)
                f.close()
                print(file)

