import os

for root, dirs, files in os.walk("."):
    for name in files:
        if name.endswith(".ixx"):
            old_path = os.path.join(root, name)
            new_path = os.path.join(root, name[:-4] + ".h")
            os.rename(old_path, new_path)
            print(f"{old_path} -> {new_path}")