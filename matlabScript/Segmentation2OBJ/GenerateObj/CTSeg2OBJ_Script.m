%% SEGMENTATION SKEL PRUNNING
volimage = AirwaySeg;
volimage = PrincipalConnComp(volimage,26,1);
w=size(volimage,1);
l=size(volimage,2);
h=size(volimage,3);


%% VOL RECONSTRUCTION FROM PRUNNED SKEL
volRecon=volimage;
%%% Save results
% OBJ
volRecon_dist=bwdist(volRecon);
pixsize=[1,1,1];
CTSeg2Obj_VTK(volRecon_dist, 1.5, [OutPutDataFolder filesep 'airwaySeg.obj'],pixsize,ROI);



