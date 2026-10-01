%==========================================================================
% FUNCTION NAME : [faceFlip]=MeshOrientation(node,face,volimage);
%--------------------------------------------------------------------------
% DESCRIPTION : Reorient faces using triangle orientation with respect closest exterior point
%
%
%--------------------------------------------------------------------------
% INPUTS :
% 1> node: NVx3 matrix of mesh vertices
% 2> face: NFx3 array of faces to be oriented
% 3> volimage: mask defining volume interior (mesh interior points)
%--------------------------------------------------------------------------
% OUTPUTS :  
% 1> faceFlip: Oriented faces
%--------------------------------------------------------------------------
% EXTERNAL FUNCTIONS :
%
%--------------------------------------------------------------------------
% RELATED BIBLIOGRAPHY :
%--------------------------------------------------------------------------
% CREATION DATE : 09 / 03 / 2016
%--------------------------------------------------------------------------
% LAST MODIFICATION :
%--------------------------------------------------------------------------
% AUTHOR : Debora Gil
%--------------------------------------------------------------------------
% USAGE EXAMPLES :
% load('LENS10_INSPI_SIN_Recon_Linear_SMOOTH.mat') 
%%%% Create volume from triangulation to ensure mesh fits boundary
% node(:,3)=node(:,3)+5;
% FV.vertices=node;
% FV.faces=face;
% volimage=polygon2voxelDeb(FV,size(volimage),'none');
% for k=1:size(volimage,3)
%     volimage(:,:,k)=imfill(volimage(:,:,k),'holes');
% end
% [faceFlip]=MeshOrientation(node,face,volimage);
%==========================================================================



function [faceFlip]=MeshOrientation(node,face,volimage)


%%% Triangle vertex coordinates
n1=node(face(:,1),:);
n2=node(face(:,2),:);
n3=node(face(:,3),:);
nM=(n1+n2+n3)/3;

%%% exterior points
minD=2;
maxD=3;
volimage=cat(3,zeros([size(volimage(:,:,1)),maxD]),volimage);
volimage=cat(1,volimage,zeros([maxD,size(volimage,2),size(volimage,3)]));
volimage=cat(2,volimage,zeros([size(volimage,1),maxD,size(volimage,3)]));

voldist=bwdist(volimage);
indIn=find((voldist(:)>minD).*(voldist(:)<=maxD));
[yIn,xIn,zIn]=ind2sub(size(volimage),indIn);
zIn=zIn-maxD;

%%% closest exterior/interior point
Nfaces=size(nM,1);
nMIn=0*nM;
for k=1:Nfaces
    dIn=(nM(k,1)-xIn).^2+(nM(k,2)-yIn).^2+(nM(k,3)-zIn).^2;
    [~,indIn]=min(dIn);
    nMIn(k,1)=xIn(indIn);
    nMIn(k,2)=yIn(indIn);
    nMIn(k,3)=zIn(indIn);
end


%%% Triangle orientation1 using
%%% Position correction
nC1=n1-nMIn;
nC2=n2-nMIn;
nC3=n3-nMIn;
det=nC1(:,1).*nC2(:,2).*nC3(:,3)+nC1(:,2).*nC2(:,3).*nC3(:,1)+nC1(:,3).*nC2(:,1).*nC3(:,2) ...
    -nC3(:,1).*nC2(:,2).*nC1(:,3)-nC2(:,3).*nC3(:,2).*nC1(:,1)-nC1(:,2).*nC2(:,1).*nC3(:,3);
indFlip=find(det>0); %% for exterior reference


%%% Face flipping
faceFlip=face;
faceFlip(indFlip,:)=face(indFlip, [2 1 3]);


