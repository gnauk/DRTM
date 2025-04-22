import pytest
import drjit as dr
import mitsuba as mi
import numpy as np

import numpy as np
import math
import matplotlib.pyplot as plt
from osgeo import gdal
import imageio

#mi.set_variant("cuda_spectral") #gpu cuda mode
#mi.set_variant("llvm_spectral") #llvm mode
mi.set_variant("scalar_spectral") #scalar mode

def saveToHdr_no_transform(npArray, dstFilePath, wlist, output_format):
    dshape = npArray.shape
    if len(dshape) == 3:
        bandnum = dshape[2]
    else:
        bandnum = 1
    # 从hdrHeaderPath中提取投影信息
    if output_format == "ENVI":
        format = "ENVI"
    else:
        format = "GTiff"
        dstFilePath += ".tif"
    driver = gdal.GetDriverByName(format)
    dst_ds = driver.Create(dstFilePath, dshape[1], dshape[0], bandnum, gdal.GDT_Float32)
    #     npArray = linear_stretch_3d(npArray)
    if bandnum > 1:
        for i in range(1, bandnum + 1):
            dst_ds.GetRasterBand(i).WriteArray(npArray[:, :, i - 1])
    else:
        dst_ds.GetRasterBand(1).WriteArray(npArray)
    dst_ds = None

    if output_format == "ENVI" and len(wlist) >0:
        # wirte wavelength
        f = open(dstFilePath+".hdr",'r')
        text = f.read()
        f.close()
        wstr = "\nwavelength = {"
        for i in range(0,len(wlist)):
            wstr += str(wlist[i]) + ","
        wstr = wstr[0:len(wstr)-1] + "}"
        f = open(dstFilePath+".hdr",'w')
        text = text + wstr
        f.write(text)
        f.close()


scene = mi.load_file('./Parameters/main-fourspecural-bilambert.xml')
image_ref = mi.render(scene)
data = image_ref[:,:,:]
np.save('temp1.npy',data)
data = np.load("temp1.npy")
bandlist = [442.948,560.4305,665.2445,865.587]
saveToHdr_no_transform(data, 'drtm-iamgery', bandlist, 'TIFF')


