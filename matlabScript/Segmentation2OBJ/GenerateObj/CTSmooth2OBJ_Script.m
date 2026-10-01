%% INPUT DATA
cd 'D:\Experiments\TestEndoscopia\Broncoscopia\Navigation\MICCAI_2016'
load('test_BronchiSeg_ori_RegalReis_LENS10_INSPI_SIN_Agnes.mat') %%% Volum Agnes Suavitzat
addpath('D:\Experiments\TestEndoscopia\Broncoscopia\Navigation\MICCAI_2016')

%% Reorient faces using triangle orientation with respect closest interior/ point
% Create volume from triangulation to ensure mesh fits boundary
FV.vertices=node;
% FV.faces=face(NF_New,:); %Sols cares conectades
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

%%% interior points
% Defined by skel
%% Better than volume but still some triangles are missed due to face points
skel=Skeleton3D(volimage);
indIn=find(skel(:));
[yIn,xIn,zIn]=ind2sub(size(volimage),indIn);

% Defined by volume
%% Some small branches are missed but lowest threshold introduces face points
voldist=bwdist(1-volimage);
indIn=find((voldist(:)>2).*(voldist(:)<=3));
[yIn,xIn,zIn]=ind2sub(size(volimage),indIn);
    
%%% exterior points
voldist=bwdist(volimage);
indIn=find((voldist(:)>2).*(voldist(:)<=3));
[yIn,xIn,zIn]=ind2sub(size(volimage),indIn);
 
%%% closest exterior/interior point
Nfaces=size(nM,1);
nMIn=0*nM;
for k=1:Nfaces
    dIn=(nM(k,1)-xIn).^2+(nM(k,2)-yIn).^2+(nM(k,3)+h-zIn).^2;
    [~,indIn]=min(dIn);
    nMIn(k,1)=xIn(indIn);
    nMIn(k,2)=yIn(indIn);
    nMIn(k,3)=zIn(indIn);
end

% Show interior/exterior points
figure,plotmesh(node,faceFlip,'edgecolor', 'none');camlight
hold on
plot3(nMIn(:,1),nMIn(:,2),nMIn(:,3),'k+');

%% OBS: Both orientations are equivalent
%%% Triangle orientation1
%%% Position correction
nC1=n1-nMIn;
nC2=n2-nMIn;
nC3=n3-nMIn;
det=nC1(:,1).*nC2(:,2).*nC3(:,3)+nC1(:,2).*nC2(:,3).*nC3(:,1)+nC1(:,3).*nC2(:,1).*nC3(:,2) ...
    -nC3(:,1).*nC2(:,2).*nC1(:,3)-nC2(:,3).*nC3(:,2).*nC1(:,1)-nC1(:,2).*nC2(:,1).*nC3(:,3);
indFlip=find(det<0); %% for interior reference
indFlip=find(det>0); %% for exterior reference

%%% Triangle orientation2
V1=n2-n1;
V2=n3-n1;
VMIn=-nMIn+nM; %%% Normal exterior
det=V1(:,1).*V2(:,2).*VMIn(:,3)+V1(:,2).*V2(:,3).*VMIn(:,1)+V1(:,3).*V2(:,1).*VMIn(:,2) ...
    -VMIn(:,1).*V2(:,2).*V1(:,3)-V2(:,3).*VMIn(:,2).*V1(:,1)-V1(:,2).*V2(:,1).*VMIn(:,3);
indFlip=find(det<0); %% for interior reference
indFlip=find(det>0); %% for exterior reference

%%% Face flipping
faceFlip=face;
faceFlip(indFlip,:)=face(indFlip, [2 1 3]);
Nflip=N(NF_New,:);
Nflip(indFlip,:)=-N(indFlip,:);

%%% Show
figure,
plotmesh(node,faceFlip,'edgecolor', 'none');
camlight

% Write obj
clear OBJ
OBJ.vertices = node;
%OBJ.vertices_normal = Nflip;
OBJ.objects(1).type='f';
OBJ.objects(1).data.vertices=faceFlip;
OBJ.objects(1).data.normal= faceFlip;
write_wobj(OBJ,'LENS10_INSPI_SIN_SMOOTH.obj');

%% METHOD1: Reorient faces using triangle normal and checking that closest
%% point along normal lies inside object mask defined by volimage
%% FAILS TO GIVE EXPECTED RESULTS

%%% Triangle normal
n1=node(face(:,1),:);
n2=node(face(:,2),:);
n3=node(face(:,3),:);
V1=n2-n1;
V2=n3-n1;
N(:,1)=V1(:,2).*V2(:,3)-V2(:,2).*V1(:,3);
N(:,2)=V1(:,3).*V2(:,1)-V2(:,3).*V1(:,1);
N(:,3)=V1(:,1).*V2(:,2)-V2(:,1).*V1(:,2);

for k=1:3
    N(:,k)=N(:,k)./sqrt(sum(N.^2,2));
end

%%% Vertex Normal
figure,
h=plotmesh(node,face,'edgecolor', 'none');
NV=get(h,'vertexnormals');

h=2;
% j=(node(:,1)+h*NV(:,1));
% i=(node(:,2)+h*NV(:,2));
% k=(node(:,3)+h*NV(:,3));

j=(nM(:,1)+h*N(:,1));
i=(nM(:,2)+h*N(:,2));
k=(nM(:,3)+h*N(:,3));
i=min(i,size(volimage,1));
i=max(1,i);
j=min(j,size(volimage,2));
j=max(1,j);
k=min(k,size(volimage,3));
k=max(1,k);

volI=interp3(volimage,j,i,k,'linear');
indFlip=find(volI(:)>0);

ind=sub2ind(size(volimage),i,j,k);
indFlip=find(volimage(ind)>0);

faceFlip=face;
face2=face(:,2);
face3=face(:,3);
faceFlip(indFlip,2)=face3(indFlip);
faceFlip(indFlip,3)=face2(indFlip);


%%% Show
figure,
plotmesh(node,faceFlip,'edgecolor', 'none');
camlight

% Write obj
clear OBJ
OBJ.vertices = node;
%OBJ.vertices_normal = Nflip;
OBJ.objects(1).type='f';
OBJ.objects(1).data.vertices=faceFlip;
OBJ.objects(1).data.normal= faceFlip;
write_wobj(OBJ,'LENS10_INSPI_SIN_SMOOTH.obj');

%% METHOD2: NORMAL CONSISTENCY 
%% Make all orientations consistent by visiting all faces
%% and reorienting according to neighnoring faces orientation

%%% Triangle normal
V1=n2-n1;
V2=n3-n1;
N(:,1)=V1(:,2).*V2(:,3)-V2(:,2).*V1(:,3);
N(:,2)=V1(:,3).*V2(:,1)-V2(:,3).*V1(:,1);
N(:,3)=V1(:,1).*V2(:,2)-V2(:,1).*V1(:,2);

for k=1:3
    N(:,k)=N(:,k)./sqrt(sum(N.^2,2));
end

%%%% Normal consistency
% List of ordered adjacent faces together with closest face to be used as
% reference for normal re-orientation
%% OBS1: due to dicret artefacts some neighbouring faces have opposite normals
%% OBS2: there are some faces that are not connected using face_ring adjacency
frings = compute_face_ring(face');
NF=1;
NF_New=NF;
ALL=1:Nfaces;
faceRef_New=[];
stop=1;
k=1;
while(stop.*(k<=Nfaces))
    faceRef=[repmat(NF_New,[3,1])];
    faceRef=faceRef(:);
    [NF,ind]=setdiff([frings{NF_New}],NF_New);
    if(isempty(ind) break; end
    faceRef_New=[faceRef_New faceRef(ind)'];
    NF_New=[NF_New NF];
    stop=1-isempty(setdiff(ALL,NF_New));
    k=k+1;
end
NF_New=NF_New(2:end);
NfacesFlip=length(NF_New);
% Flip faces according to closest normal orientation:
% NF_New contains all faces connected by ring connectivity
facesFlip=face;
Nflip=N;
indFlip=zeros(1,Nfaces);
for k=1:NfacesFlip
    %%% Flip condition
    NfaceRef=N(faceRef_New(k),:);
    Nface=N(NF_New(k),:);
    indFlip(NF_New(k))=sign(Nface*NfaceRef');
        if(indFlip(NF_New(k))<0)
            
            faceFlip(NF_New(k),:)=face(NF_New(k), [2 1 3]);
            Nflip(NF_New(k),:)=-N(NF_New(k),:);
        end
end
indFlip=indFlip(NF_New);
facesFlip=[face(1,:); facesFlip(NF_New,:)];
Nflip=[N(1,:); N(NF_New,:)];


%%% Show
figure,
plotmesh(node,faceFlip,'edgecolor', 'none');
camlight

% Write obj
clear OBJ
OBJ.vertices = node;
%OBJ.vertices_normal = Nflip;
OBJ.objects(1).type='f';
OBJ.objects(1).data.vertices=faceFlip;
OBJ.objects(1).data.normal= faceFlip;
write_wobj(OBJ,'LENS10_INSPI_SIN_SMOOTH.obj');

