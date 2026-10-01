function [ROI,G]=DistalRegion4LocRefinement_29_12_2017(DistalGlobalTh,Leakage,InputParam,varargin);

MidTh=InputParam.MidTh;
CloseSze=InputParam.CloseSze;
ThA=InputParam.ThA;
if(isempty(varargin))
    % Add 26 Branches 2 6 Conn Segmentation
    % DistalIni Skeleton (Es millor recalcular-ho sobre tot pq a trossos no
    % codifica be els punts finals pq la f x calcular l'esquelet es una
    % merdeta)
    skelDistal=Skeleton3D(DistalGlobalTh);
    [ Adj,node,link] = Skel2Graph3D_Deb_07_06_2017(skelDistal,0);
    G = GraphAdapt(node,link,Adj);
else
    G=varargin{1};
end

[TBMask]=TracheaBronchiSep_Agnes(DistalGlobalTh);
TBDist=bwdist(TBMask)>0;
DistSeg=bwdist(1-DistalGlobalTh);

% DistalIni EndPoints
sze=size(DistalGlobalTh);
EEPt=[];
EE0=find(G.wv(:,1)==0);
% MidNodes
MidNodes= find((sum(G.e>0,2)==2)>0);

% EE No MidNodes
EE=setdiff(EE0,MidNodes);
for k=1:length(EE)
    
    EEi=EE(k);
    % Node Points
    Nodeind=sub2ind(sze,round(G.v(EEi,1)),round(G.v(EEi,2)),round(G.v(EEi,3)));
    EEPt=[EEPt Nodeind];
    % Skel EndPoints
    EEPt=[EEPt [G.lp{EEi,:}]];
    
end
% MidNodes
L=[];
s=1;
EE=intersect(EE0,MidNodes);
for k=1:length(EE)
    
    EEi=EE(k);
    BranchL=0;
    indLP=find(1-(cellfun('isempty',{G.lp{EEi,:}})));
    NB=length(indLP);
    
    Nodeind=sub2ind(sze,round(G.v(EEi,1)),round(G.v(EEi,2)),round(G.v(EEi,3)));
    
    for kB=1:NB
        [x,y,z]=ind2sub(sze,G.lp{EEi,indLP(kB)});
        BranchL=BranchLength(x,y,z);
     
        if((BranchL-DistSeg(Nodeind))>MidTh)
            % Node Points
            %  sIn=[sIn,s];
            EEPt=[EEPt Nodeind];
            % Skel EndPoints
            EEPt=[EEPt [G.lp{EEi,indLP(kB)}]];
        end
        s=s+1;
    end
end

DistBranchEE=0*DistalGlobalTh;
DistBranchEE(EEPt)=1;
DistBranchEE=DistBranchEE.*(TBDist);
DistBranchEE=bwdist(DistBranchEE)<=1;


% Distal Region Excluding Leakage
ROI=(max(Leakage,PrincipalConnComp(DistalGlobalTh.*(1-DistBranchEE),26,1)));
ROI=bwdist(ROI)>1;
% Remove fake branches
ROI=bwdist(DistalGlobalTh.*(1-ROI))>CloseSze;
ROI=bwdist(ROI)>CloseSze;
ROI=1-ROI;

DistBranchEE=bwlabeln(DistalGlobalTh.*ROI);
p=regionprops(DistBranchEE);
indL=find([p(:).Area]>ThA);
DistBranchEE=ismember(DistBranchEE,indL);
DistBranchEE=bwdist(DistBranchEE)<=1;
ROI=(max(Leakage,PrincipalConnComp(DistalGlobalTh.*(1-DistBranchEE),26,1)));
ROI=bwdist(ROI)>1;