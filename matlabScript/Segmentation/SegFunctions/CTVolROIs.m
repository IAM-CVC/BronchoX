function [ROI ] = CTVolROIs( LungMask )


h=2;
%Lung ROI Computation
LungMaskLA=squeeze(sum(LungMask,1)>0);
LAProfile=sum(LungMaskLA,1);
Th=prctile(LAProfile(find(LAProfile>0)),10);
[k]=find(LAProfile>Th);
ROI.k(1)=k(1);
ROI.k(2)=k(end);
LungMaskSA=squeeze(sum(LungMask,3)>0);
[i]=find(sum(LungMaskSA,2)>0);
ROI.i(1)=max(1,i(1)-h);
ROI.i(2)=min(i(end)+h,size(LungMask,1));
[j]=find(sum(LungMaskSA,1)>0);
ROI.j(1)=max(1,j(1)-h);
ROI.j(2)=min(j(end)+h,size(LungMask,2));
