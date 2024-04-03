import gdown
import os
import pathlib


def download(id):
    gdown.download(id=id, output="data.7z")
    os.system("7z x data.7z -odata")
    pathlib.Path("data.7z").unlink()


download("1RmT19FCH3gnxTi8GRCrI_UUJGSMPUXTT")
download("1oubDMtxYPeT3VOT52JTZ-3gddrBX8aAQ")
