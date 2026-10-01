function [LungMask]=VolumeMaskClose(LungMaskROIHoles,d_se)

%%% Manage Boundary values
LungMaskROIHoles_Plus=zeros(size(LungMaskROIHoles)+2*d_se);
LungMaskROIHoles_Plus(d_se+1:end-d_se,d_se+1:end-d_se,d_se+1:end-d_se)=LungMaskROIHoles;

LungMaskDist=bwdist(LungMaskROIHoles_Plus); 
%%% Closing arteries and bronchial walls
LungMaskClosing=double(LungMaskDist<d_se);
LungMaskDist=bwdist(1-LungMaskClosing);
LungMask=double(LungMaskDist>=d_se);

LungMask=LungMask(d_se+1:end-d_se,d_se+1:end-d_se,d_se+1:end-d_se);