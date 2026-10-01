function [ VesselSeg ] = VesselFilter( VesselSeg,LungMasks,FiltParam )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

LungMaskROIL=LungMasks.LungMaskROIL;
LungMaskROIR=LungMasks.LungMaskROIR;
LungMask_dse=FiltParam.LungMask_dse;

LungMaskDistL=bwdist(1-LungMaskROIL);
LungMaskDistR=bwdist(1-LungMaskROIR);
LungMaskROI=(LungMaskDistL>LungMask_dse)+(LungMaskDistR>LungMask_dse);

VesselSeg=PrincipalConnComp(VesselSeg,26,2);
VesselSeg=(VesselSeg>0).*LungMaskROI;
end

