%% INPUT DATA
clear all

machine='PCDeb'
switch machine
    case 'PCDeb'
        cd 'D:\Experiments\TestEndoscopia\Broncoscopia\Navigation\MICCAI_2016'
        FileName='LENS10_INSPI_SIN_Recon_Linear_SMOOTH';
        load([FileName '.mat']) %%% Volum to ReOrient
        node=node(:,[2,1,3]); % Restore x-y orientation since node is in i-j matrix coordinates
        addpath('D:\Experiments\TestEndoscopia\Broncoscopia\Navigation\MICCAI_2016')
    case 'PCAg'
        ;
end

Show=0;

%% Reorient faces using triangle orientation with respect closest interior/ point
% Create volume from triangulation to ensure mesh fits boundary
node(:,3)=node(:,3)+5;
FV.vertices=node;
FV.faces=face;
volimage=polygon2voxelDeb(FV,size(volimage),'none');
for k=1:size(volimage,3)
    volimage(:,:,k)=imfill(volimage(:,:,k),'holes');
end

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
% for k=1:3
%     nMIn(:,k)=nMIn(:,k)-maxD;
% end

% Show interior/exterior points
if(Show)
    figure,plotmesh(node,face,'edgecolor', 'none');camlight
    hold on
    plot3(nMIn(:,1),nMIn(:,2),nMIn(:,3),'k+');
end
%% OBS: Both orientations are equivalent
%%% Triangle orientation1 using
%%% Position correction
nC1=n1-nMIn;
nC2=n2-nMIn;
nC3=n3-nMIn;
det=nC1(:,1).*nC2(:,2).*nC3(:,3)+nC1(:,2).*nC2(:,3).*nC3(:,1)+nC1(:,3).*nC2(:,1).*nC3(:,2) ...
    -nC3(:,1).*nC2(:,2).*nC1(:,3)-nC2(:,3).*nC3(:,2).*nC1(:,1)-nC1(:,2).*nC2(:,1).*nC3(:,3);
indFlip=find(det>0); %% for exterior reference

%%% Triangle orientation2
% V1=n2-n1;
% V2=n3-n1;
% VMIn=-nMIn+nM; %%% Normal exterior
% det=V1(:,1).*V2(:,2).*VMIn(:,3)+V1(:,2).*V2(:,3).*VMIn(:,1)+V1(:,3).*V2(:,1).*VMIn(:,2) ...
%     -VMIn(:,1).*V2(:,2).*V1(:,3)-V2(:,3).*VMIn(:,2).*V1(:,1)-V1(:,2).*V2(:,1).*VMIn(:,3);
% indFlip=find(det<0); %% for interior reference
% indFlip=find(det>0); %% for exterior reference

%%% Face flipping
faceFlip=face;
faceFlip(indFlip,:)=face(indFlip, [2 1 3]);

save([FileName '.mat'],'face','faceFlip','node','volimage');

%%% Show
if(Show)
    figure,
    plotmesh(node,faceFlip,'edgecolor', 'none');
    camlight
end

% Write obj
clear OBJ
pixsize= [0.6890 0.6890 0.5000];
node(:,3)=node(:,3)-5;
OBJ.vertices(:,1) = node(:,2);
OBJ.vertices(:,2) = node(:,1);
OBJ.vertices(:,3) = node(:,3);
for k=1:3
    OBJ.vertices(:,k)=OBJ.vertices(:,k)*pixsize(k);
    
end

OBJ.objects(1).type='f';
OBJ.objects(1).data.vertices=faceFlip;
OBJ.objects(1).data.normal= faceFlip;
write_wobj(OBJ,[FileName '.obj']);

figure,
    plotmesh(OBJ.vertices,faceFlip,'edgecolor', 'none');
    camlight