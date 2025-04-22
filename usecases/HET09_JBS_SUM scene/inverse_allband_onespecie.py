#!/usr/bin/env python
# coding: utf-8

import drjit as dr
import mitsuba as mi
import numpy as np
import csv
from sklearn.metrics import r2_score
from npy_append_array import NpyAppendArray
mi.set_variant("llvm_ad_spectral")
#mi.set_variant("cuda_ad_spectral")

scene = mi.load_file('../Parameters/main-fourspecural-bilambert-inverse.xml')
# or use the imagey generated use other models  or with the measured imagery
image_ref = mi.render(scene)


def mse(image):
    return dr.mean(dr.sqr(image - image_ref))*1e8

def mse_deng(image1,image2):
    return dr.mean((image1-image_ref)*(image2-image_ref))

def iterate():
    params = mi.traverse(scene)

    key1 = 'TICO1_foliage_copy.reflectance.values'
    key2 = 'TICO1_foliage_copy.transmittance.values'

    param1_ref = mi.Float(params[key1])
    param2_ref = mi.Float(params[key2])

    import numpy as np
    reflect=np.load('reflect2.npy')
    trans=np.load('trans2.npy')
    params[key1] = mi.Float(reflect[-4:])
    params[key2] = mi.Float(trans[-4:])
    params.update();
    print(param1_ref,param2_ref)
    print(params[key1],params[key2])

    temp1=dr.enable_grad(params[key1])
    temp2=dr.enable_grad(params[key2])
    image_init = mi.render(scene, params)

    opt = mi.ad.Adam(lr=0.05)
    opt[key1] = params[key1]
    opt[key2] = params[key2]
    #opt.set_learning_rate({key1:0.01,key2:0.01})
    params.update(opt)

    iteration_count = 50 #depend on the computer setting.
    totalerrors = []
    parameter1errors = np.empty(shape=(1,4))
    parameter2errors = np.empty(shape=(1,4))
    lossarr=[]
    for it in range(iteration_count):
        
        image = mi.render(scene, params)
        #or the mse_deng
        #image1 = mi.render(scene, params)
        #loss =mse_deng(image,image1)
        
        loss = mse(image)
        dr.backward(loss)
        opt.step()
        
        opt[key1] = dr.clamp(opt[key1], 0.0001, 1.0)
        opt[key2] = dr.clamp(opt[key2], 0.0001, 1.0)

        params.update(opt)

        # Track the difference between the current color and the true value
        err_ref = dr.sum(dr.abs(param1_ref - params[key1]))
        err_ref += dr.sum(dr.abs(param2_ref - params[key2]))
        print(f"Iteration {it:02d}: loss error= {loss}, parameter error = {err_ref[0]:6f}, key1={params[key1]},key2={params[key2]}")
        totalerrors.append(err_ref)
        parameter1errors=np.append(parameter1errors,np.array(param1_ref - params[key1]).reshape(1,4),axis = 0)
        parameter2errors=np.append(parameter2errors,np.array(param2_ref - params[key2]).reshape(1,4),axis = 0)
        lossarr.append(loss)
        
        with NpyAppendArray('reflect.npy') as npaa:
            npaa.append(np.array(params[key1]).reshape(1,4).flatten())
        with NpyAppendArray('trans.npy') as npaa1:
            npaa1.append(np.array(params[key2]).reshape(1,4).flatten())
        with NpyAppendArray('loss.npy') as npaa2:
            npaa2.append(np.array(loss).flatten())

    print('\nOptimization complete.')
    print('ref:',param1_ref,param2_ref)
    print('inverse:',params[key1],params[key2])
    
iterate()

