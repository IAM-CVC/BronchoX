function [ LungMaskROIAll,LungMaskROIAllHoles,LungMaskROIL,LungMaskROIR,TracheaMaskROI,LugSegParam,TracheaEner] = CTLungSeg_09_05_2017( imaVOLROI,BodyMaskROI,LugSegParam,varargin )

d_se= LugSegParam.d_seTrachea;
imaVOLROI=imaVOLROI.*BodyMaskROI;

%% Lung Energy
N=1;
hTh=pi/N;
FParam.sig=[1];
FParam.scale=1;
FParam.Th1=[[0:1:N-1]*hTh];
FParam.Th2=[[0:1:N-1]*hTh]';
FParam.offset=[1,1,1];
FParam.FiltType='DNR_Bola_GPU';
FParam.ConvType='Fourier';
[ EnerL ] = IntensityEner( imaVOLROI,FParam );

%% Lung-MainAirways Mask
%Aside from the main bronchi, this mask also contains the boundaries between arteries/vessels and lung
% LungMaskROIHoles is a mask of the lung excluding arteries and bronchial
% walls
th_DNR2_Neg=graythresh(EnerL(find(EnerL)));
LungMaskROIAllHoles=PrincipalConnComp(double(EnerL>th_DNR2_Neg),6,1);
[LungMaskROIAll]=VolumeMaskClose(LungMaskROIAllHoles,d_se);

SepDistParam=LugSegParam.SepDistParam;
[LungMaskROIL,LungMaskROIR,SepDistParam]=LungSeparation(LungMaskROIAllHoles,SepDistParam);
%% OBS: Potser caldria cridar la funció amb LungMaskROIAll per evitar que la traquea incorpori parts del pulmo
%% [LungMaskROIL,LungMaskROIR,SepDistParam]=LungSeparation(LungMaskROIAll,SepDistParam);

LugSegParam.SepDistParam=SepDistParam;
LungMaskROI=max(LungMaskROIL,LungMaskROIR);

%% Trachea
%% OBS: We are assuming that the patient has 2 lungs. In case of 1 lung only 1 component should be considered
%% to differentiate between 1/2 lungs use ROI.j in comparison to the whole volume or to the ROI that you woud obstain from the body mask

for k=1:size(LungMaskROIAllHoles,3)
    SA=(LungMaskROIAllHoles(:,:,k)).*BodyMaskROI(:,:,k);
    SA=bwmorph(SA,'erode');
    Mask2(:,:,k)=PrincipalConnComp(SA,6,2);
    Mask2(:,:,k)=bwmorph(Mask2(:,:,k)>0,'dilate');
end


TracheaMask=0*Mask2;
NoLungMask=(1-(Mask2(:,:,round(end/2):end)>0)).*(LungMaskROIAllHoles(:,:,round(end/2):end)).*BodyMaskROI(:,:,round(end/2):end);
NoLungMask=bwdist(1-NoLungMask)>d_se;
TracheaMask(:,:,round(size(Mask2,3)/2):end)=PrincipalConnComp(NoLungMask,26,1);
TracheaMask=bwdist(TracheaMask)<=d_se;


if(isempty(varargin))
    N=1;
    hTh=pi/N;
    FParam.sig=[1];
    FParam.scale=1;
    FParam.Th1=[[0:1:N-1]*hTh];
    FParam.Th2=[[0:1:N-1]*hTh]';
    FParam.offset=[2,1,1];
    FParam.FiltType='DNR_Bola_GPU';
    FParam.ConvType='Fourier';
    imaVOLEner=IntensityEner(imaVOLROI,FParam);
else
    imaVOLEner=varargin{1};
end

%% OBS: Això abans estava calculat així:
Th=graythresh(imaVOLEner(find(TracheaMask(:))));

%Th=prctile(imaVOLEner(find(TracheaMask(:))),50);
TracheaMaskROIRef=max((1-LungMaskROI).*LungMaskROIAllHoles,TracheaMask);
TracheaMaskROIRef=TracheaMaskROIRef.*(imaVOLEner>Th);
TracheaMaskROIRef=max(TracheaMaskROIRef,TracheaMask);
TracheaMaskROIRef=bwlabeln(TracheaMaskROIRef,6);
TracheaMaskROIRef=TracheaMaskROIRef==mode(TracheaMaskROIRef(find(TracheaMask(:))));
TracheaMaskROI=PrincipalConnComp(TracheaMaskROIRef,6,1);

LugSegParam.TracheaTh=Th;


end


