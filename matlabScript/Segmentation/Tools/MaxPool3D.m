function [ volRed ] = MaxPool3D( imaVOLROI,ratio )

[volRed, ~] = convnet_maxpool (imaVOLROI, [ratio]);
volRed2D=volRed;
%3D
clear volRed
start=floor(ratio/2);
kRed=1;
for k=1:ratio:size(imaVOLROI,3)-ratio+1
   volRed(:,:,kRed)=max(volRed2D(:,:,k:k+ratio-1),[],3);
   kRed=kRed+1;
end

end

