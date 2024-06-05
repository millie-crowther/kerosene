import subprocess

def main():
    subprocess.run(["gcc", "main.c"])
    subprocess.run(["./a.out"])

if __name__ == "__main__":
    main()
