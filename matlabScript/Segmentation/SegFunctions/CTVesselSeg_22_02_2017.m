function [ VesselSeg, VesselSegHoles,imaVOLEner] = CTVesselSeg_22_02_2017( imaVOLROI,LungMaskROI,LungMaskROIHoles )

%Large Vessels
N=6;
hTh=pi/N;
FParam.sig=[1];
FParam.scale=1;
FParam.Th1=[[0:1:N-1]*hTh];
FParam.Th2=[[0:1:N-1]*hTh]';
FParam.offset=[2,1,1];
FParam.ConvType='Fourier';
FParam.FiltType='DNR_Bola_GPU';
[ imaVOLEner] = IntensityEner( imaVOLROI,FParam );

th_DNR2_Neg=graythresh(imaVOLEner(find(LungMaskROI)));
VesselSeg=(imaVOLEner<th_DNR2_Neg).*LungMaskROI;
[VesselSeg]=VessLargestComp(VesselSeg,imaVOLROI);

%Preserva millor les parets bronquials pq 1-LungMaskROIHoles sols conte venes i parets bronquials,pero subestima una mica les venes
th_DNR2_Neg=graythresh(imaVOLEner(find((1-LungMaskROIHoles).*LungMaskROI.*(imaVOLEner>0))));
VesselSegHoles=(imaVOLEner<th_DNR2_Neg).*LungMaskROI;




end

function[TubeVessel]=VessLargestComp(TubeVessel,imaVOLROI)


[L]=bwlabeln(single(TubeVessel),6);
NLab=hist(L(find(L)),max(L(:)));
[NLab,indNLab]=sort(NLab,'descend');
TubeVessel=(L==indNLab(1)) | (L==indNLab(2));
%%% Remove possible response on bronchial wall
TubeVessel=(TubeVessel.*(imaVOLROI>-200));
TubeVessel=bwdist(TubeVessel)<1;
end