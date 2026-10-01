function [BronchiSegL,Leakage]=LeakageRemoval_Agnes(BronchiSeg,GSub,nodes_elim_all,Conn)




BronchiSegL.All=BronchiSeg;
BronchiSegL.Main26=BronchiSeg;
BronchiSegL.Main6=PrincipalConnComp( BronchiSegL.All,6,1);
BronchiSegL.Main18=PrincipalConnComp( BronchiSegL.All,18,1);
Leakage=0*BronchiSeg;

sze=size(BronchiSeg);

%% Leakage skelpoints

PtsLeak=[];
PtsBranca=[];

NGSub=length(GSub);


for k=1:NGSub
    TargetG=GSub{k};
    
    %%% Punts del Leakage
    %Edge Points
    if(isfield(TargetG, 'we'))
        PtsLeak=[PtsLeak [TargetG.we{nodes_elim_all{k},nodes_elim_all{k}}]];
    end
    %Node
    Nodeind=sub2ind(sze,round(TargetG.v(nodes_elim_all{k},1)),round(TargetG.v(nodes_elim_all{k},2)),round(TargetG.v(nodes_elim_all{k},3)));
    PtsLeak=[PtsLeak Nodeind(:)'];
    % Leaf
    PtsLeak=[PtsLeak [GSub{k}.lp{nodes_elim_all{k},:}]];
    
    %%% Resta de Punts
    IndRest=setdiff([1:size(TargetG.e,1)],nodes_elim_all{k});
    %Edge Points
    if(isfield(TargetG, 'we'))
        PtsBranca=[PtsBranca [TargetG.we{IndRest,IndRest}]];
    end
    %Node
    Nodeind=sub2ind(sze,round(TargetG.v(IndRest,1)),round(TargetG.v(IndRest,2)),round(TargetG.v(IndRest,3)));
    PtsBranca=[PtsBranca Nodeind(:)'];
    % Leaf
    PtsBranca=[PtsBranca GSub{k}.lp{IndRest,:}];
    
end

[TBMask]=TracheaBronchiSep_Agnes(BronchiSeg);
[~, Leakage] = f_reconstruct_branches(PtsBranca,PtsLeak, BronchiSeg.*(1-TBMask));


BronchiSegLAll=BronchiSeg.*(1-Leakage);
BronchiSegL=PrincipalConnComp( BronchiSegLAll,Conn,1);


end