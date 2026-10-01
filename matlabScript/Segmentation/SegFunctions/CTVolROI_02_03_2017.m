% [ ROI,imaVOLROI ] = CTVolROI_02_03_2017( imaVOL,BodyMask,LugSegParam )
% LugSegParam.LungHU=[-600,-950]; Haunsfield Units for the Lung
% LugSegParam.d_se=10; % For .5 slices
% LugSegParam.d_se=5; % For 1.0 slices

function [ ROI,imaVOLROI ] = CTVolROI_02_03_2017( imaVOL,BodyMask,LugSegParam )
%% Lung Rough Segmentation
[ LungMask] = CTLungRoughSeg( imaVOL.*BodyMask,LugSegParam );
%%Lung ROI Computation
[ROI ] = CTVolROIs( LungMask );
% ROI Cropping
imaVOLROI=imaVOL(ROI.i(1):ROI.i(2),ROI.j(1):ROI.j(2),ROI.k(1):ROI.k(2));


end

function [ LungMaskClosing ] = CTLungRoughSeg( imaVOLROI,LugSegParam )

d_se= LugSegParam.d_seROI;
LungHU=LugSegParam.LungHU;

%% OBS: We assume both lungs are connected through main bronchi. I am not 100%
%% sure this always holds
%% using this thresholding. To make sure, the difference in volume between the 2
%% main labels should be considered to decide whether 1 or 2 have to be considered

[LungConnComp,NComp]=bwlabeln(double((imaVOLROI<=LungHU(1)).*(imaVOLROI>= LungHU(2) )),26);

%% Aside from the main bronchi, this mask also contains the boundaries between arteries/vessels and lung
NLab=hist(LungConnComp(find(LungConnComp)),max(LungConnComp(:)));
[NLab,indNLab]=sort(NLab,'descend');
if(NLab(2)/NLab(1)>.9)
LungLab=indNLab([1:2]);
else
  LungLab=indNLab(1);
end

LungMask=double(ismember(LungConnComp,LungLab));
LungMaskDist=bwdist(LungMask);
%%% Closing
LungMaskClosing=double(LungMaskDist<d_se);
LungMaskDist=bwdist(1-LungMaskClosing);
LungMaskClosing=double(LungMaskDist>d_se);


end

