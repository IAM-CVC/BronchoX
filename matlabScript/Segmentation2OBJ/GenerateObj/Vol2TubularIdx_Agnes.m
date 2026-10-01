function [indRecon_RProj]=Vol2TubularIdx_Agnes( linkP,voldist )

fact=1.1;
w=size(voldist,1);
l=size(voldist,2);
h=size(voldist,3);

for k=1:length(linkP)
    RMx(k)=max(voldist(linkP(k).point));
    Rm(k)=min(voldist(linkP(k).point));
end

%RMx=(RMx+Rm)/2;
[RMxUni,indUni]=unique(RMx);
RmUni=Rm(indUni);

indRecon_RProj=[];
for k=1:length(RMxUni)
    % Branches to reconstruct
    indRMx=find(RMx==RMxUni(k));
    % Distance to branches
    volBranch=0*voldist;
    volBranch([linkP(indRMx).point])=1;
    volBranch=bwdist(volBranch);
    
    %R_Branch=(RMxUni(k)+RmUni(k))*.5;
    R_Branch=RMxUni(k);
    indRecon_R=find(volBranch(:)<=R_Branch*fact);
    [indRecon_R ]=AddProjConstrain(indRecon_R,linkP(indRMx),[w,l,h]);
    indRecon_RProj=[indRecon_RProj(:) ;indRecon_R];
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [indRecon_RProj]=AddProjConstrain(indRecon_R,linkP,sizeIm)

NProj=length(linkP);
indRecon_RProj=[];
[y,x,z]=ind2sub( sizeIm,indRecon_R);

for k=1:NProj
    [yP,xP,zP]=ind2sub( sizeIm,(linkP(k).point));
    V=[xP(end)-xP(1),yP(end)-yP(1),zP(end)-zP(1)];
    V=V/norm(V);
   
    Px=x-xP(1);
    Py=y-yP(1);
    Pz=z-zP(1);
    Proj1=Px.*V(1)+Py.*V(2)+Pz.*V(3); %%% Proj1 should be positive
     
    Px=x-xP(end);
    Py=y-yP(end);
    Pz=z-zP(end);
    Proj2=Px.*V(1)+Py.*V(2)+Pz.*V(3); %%% Proj2 should be negative
    
    indRecon_RProj=[indRecon_RProj; indRecon_R(find((Proj2(:)<=0).*(Proj1(:)>=0)))];
end