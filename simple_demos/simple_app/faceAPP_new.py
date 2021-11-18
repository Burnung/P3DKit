import glob
from math import exp
from os.path import isdir
import sys
import os
from types import new_class
from numpy.core.defchararray import _to_string_or_unicode_array
from numpy.core.fromnumeric import reshape, resize

from numpy.lib import utils
from scipy.optimize._trustregion_constr.tr_interior_point import tr_interior_point
from scipy.optimize.linesearch import scalar_search_armijo
#sys.path.append('pyP3D')
import pyP3D.pyP3D as pyP3D
import numpy as np
import cv2
#import utils.face3d_data as face3d_data
import utils.animoji as animoji
import utils.utils as util
import utils.io_utils as io_utils
import shutil
import json
from face3D.kfm_face3D_solver_new import KFMFace3DSolver
from face3D.kfm_opt_data import KfmOptData
import cv2
import time



def init_pyApp(w,h,scene_cfg,redner_cfg,ui_cfg):
    app = pyP3D.PyApp()
    app.init(w,h)
    app.set_cfgs(scene_cfg,redner_cfg,ui_cfg)
    return app

def src_ani_show():
    
    app = init_pyApp(all_imgs.shape[1],all_imgs.shape[0]//2,'',render_cfg,ui_cfg)
    #face3d_solver.calc(all_imgs)

    vps = face3d_solver.get_face_mesh_vps()
    tls = face3d_solver.get_face_mesh_tl()
    uvs = face3d_solver.get_face_mesh_uv()

    app.set_tex('src_img',np.flip(cv2.cvtColor(all_imgs[-1],cv2.COLOR_BGR2RGB),0))
    face_tex = cv2.imread('resource/tex.jpg')
    app.set_tex('face_tex',face_tex)

    app.set_values(ani_factor.key_list,t_ani.tolist())
    dst_dir = r'D:\work\data\animojiData\WangTiger\pick_ok\fit2_0'
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)
    for i in range(len(all_imgs)):
        timg = all_imgs[i]
        app.set_tex('src_img',np.flip(cv2.cvtColor(timg,cv2.COLOR_BGR2RGB),0))
        tlds = all_lds[i]
        #face3d_solver.calc(all_imgs[i])
        #print(i,'pos 3d is ',face3d_solver.get_rt())
        #calc_lds = face3d_solver.get_calc_lds()
        calc_lds = all_calc_lds[i]

        #t_ani = np.array(ani_infos[i]['animoji'])
        #ani_vps = ani_factor.apply(t_ani)*10
        #ani_vps = ani_vps.flatten().tolist()
        #t_vps = face3d_solver.get_face_mesh_vps()
        t_vps = all_vps[i]
        t_mat = all_face_mats[i]
        #t_mat = face3d_solver.get_face_mat()
        for tp in tlds:
            cv2.circle(timg,(int(tp[0]),int(tp[1])),3,(0,255,0),-1)
        for tp in calc_lds:
            cv2.circle(timg,(int(tp[0]),int(tp[1])),3,(255,0,0),-1)
        app.set_tex('lds_img',np.flip(cv2.cvtColor(timg,cv2.COLOR_BGR2RGB),0))
        #app.add_comp_data('face',ani_vps,src_tl,src_uvs)
        #app.add_comp_data('face',all_vps[i],tls,uvs)
        #app.set_comp_mat('face',all_face_mats[i])
        app.add_comp_data('face',t_vps,tls,uvs)
        app.set_comp_mat('face',t_mat)
        app.render()
        #ret_img = app.get_render_img_u8('merge_fbo',0)
        ret_img = app.get_render_img_u8('forwardFbo',0)
        ret_img = cv2.flip(cv2.cvtColor(ret_img,cv2.COLOR_RGBA2BGR),0)
        cv2.imwrite(os.path.join(dst_dir,'frame_{}.png'.format(i)),ret_img)
    
    #app.run_win()
    
