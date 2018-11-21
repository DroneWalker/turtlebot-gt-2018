import os

def main():
    i = 89

    for filename in os.listdir("Photos/"):
        name = str(i) + ".png"
        src = "Photos/" + filename
        dst = "Photos/" + name

        os.rename(src,dst)
        i += 1


if __name__ == "__main__":
    main()