function []=CTSeg2Obj_VTK(Target,isoVal,FileOut,pixsize,ROI)

%% extract mesh from volume

[M,N,P]=size(Target);
[F, V] = isosurface(Target,isoVal);
N=isonormals(Target,V);



%% write obj in VTK coordinate system
V0(:,1)=V(:,1)+ROI.j(1)-1;
V0(:,2)=V(:,2)+ROI.i(1)-1;
V(:,3)=V(:,3)+ROI.k(1)-1;

 V(:,1)=V0(:,2);
 V(:,2)=V0(:,1);

for k=1:3
    VSc(:,k)=V(:,k)*pixsize(k);
end
%%% Change vertex face order
FNormal(:,1)=F(:,1);
FNormal(:,2)=F(:,3);
FNormal(:,3)=F(:,2);

OBJ.vertices = VSc;
OBJ.vertices_normal = -N;
OBJ.objects(1).type='f';
OBJ.objects(1).data.vertices=F;
OBJ.objects(1).data.normal=FNormal;
write_wobj(OBJ,FileOut);
