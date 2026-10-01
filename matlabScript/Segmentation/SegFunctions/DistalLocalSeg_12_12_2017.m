function [ DistalBronchi] = DistalLocalSeg_12_12_2017(InputMasks,BronchiEner,BronchiWallEner,SegParam )

%Masks
DistalRegions=InputMasks.DistalReg;
MainBronchi=InputMasks.MainBronchi;

DistalBronchi=0*BronchiEner;

% Distal Local Multithresholding
NReg=length(SegParam.ThAll);
for k=1:NReg
    Th=SegParam.ThAll(k);
    BronchiEReg=BronchiEner.*DistalRegions{k};
    SegIni=MainBronchi;
    SegTmp=double(max(BronchiEReg>Th,SegIni));
    SegTmp=bwlabeln(SegTmp,26);
    SegTmp=ismember(SegTmp,SegTmp(find(SegIni(:))));
    DistalBronchi=max(DistalBronchi,SegTmp);
end

% Remove wall response
SegDist=bwdist(DistalBronchi);
WallMask_ori=(BronchiWallEner>SegParam.ThWall).*(SegDist<=SegParam.ThBronchiDist);
DistalBronchi=PrincipalConnComp((1-WallMask_ori).*DistalBronchi,26,1);

end



