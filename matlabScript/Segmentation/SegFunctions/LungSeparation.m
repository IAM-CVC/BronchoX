function [LungMaskL,LungMaskR,SepDistParam]=LungSeparation(LungMaskROIHoles,SepDistParam)

%d=5;

SepCond=1;

while(SepCond>.5)
    %Separate Lungs
    Mask2=bwdist(1-LungMaskROIHoles)>SepDistParam.d;
    Mask2=PrincipalConnComp(Mask2,6,2);
    MaskL=Mask2==1;
    MaskR=Mask2==2;
    %Separation Condition
    SepCond=abs(sum(MaskL(:))-sum(MaskR(:)))/(sum(MaskL(:))+sum(MaskR(:)));
    SepDistParam.d=SepDistParam.d+1;
end

% Identify each lung
X=Mask2(:,1:round(end/2),:);
LungMaskL=double(Mask2==mode(X(find(X))));
LungMaskR=Mask2.*(1-LungMaskL);


%Restore original volumes and close holes
[LungMaskL]=LungClose(LungMaskL,SepDistParam);
[LungMaskR]=LungClose(LungMaskR,SepDistParam);



function [LungMask]=LungClose(LungMaskL,SepDistParam)

d=SepDistParam.d;
d_se=SepDistParam.d_se;

LungMaskL=bwdist(LungMaskL)<=d;

% Close Holes
LungMaskDist=bwdist(LungMaskL);

%%% Closing arteries and bronchial walls
LungMaskClosing=double(LungMaskDist<d_se);
LungMaskDist=bwdist(1-LungMaskClosing);
LungMask=double(LungMaskDist>=d_se);

for k=1:size(LungMask,3) 
    LungMask(:,:,k)=imfill(LungMask(:,:,k),'holes'); 
end

