function [ BodyMask ] = BodyMask( imaVOLROI,LugSegParam )

BodyHU=LugSegParam.BodyHU;
BodyMask=imaVOLROI>BodyHU; %-100; 
se=strel('disk',1);
for k=1:size(imaVOLROI,3) 
    BodyMask(:,:,k)=imclose(BodyMask(:,:,k),se);
    BodyMask(:,:,k)=imfill(BodyMask(:,:,k),'holes'); 
end



end

