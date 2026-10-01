function [success] = ExportClassificationScores2ExcelFile( OutExcelFile,DataExp)


success=1;
Header={'Video_ID','Im_ID','Style','Epoc', 'Sigmoid'};
Column={'A','B','C','D','E'}
CNN_Mix={'cycle','u-net','block1'}
Epoc=[21,50,200]

cd(DataExp);



SigData=load([DataExp.File]);
DataFields=fieldnames(SigData);
indSig=find(1-cellfun(@isempty,strfind(DataFields,'Sig')));
SigData=getfield(SigData,DataFields{indSig});



NSamples=length(SigData.virtuales(1:5:end,1));
VideoID=[];
for k=1:length(SigData.virtuales(:,1))/500
    VideoID=[VideoID k*ones(1,500)];
end
VideoID=VideoID(1:5:end);
ImID=1:NSamples;
%DataSamp=[[401:500],[901:1000],[1401:1500],[1901:2000]];

success=xlswrite([OutExcelFile],Header,'A1:E1');
ExcelLine=3;
for k=1:length(Epoc)
    ClassData=[SigData.cycleGAN21(1:5:end,2)',SigData.cycleGAN50(1:5:end,2)',SigData.cycleGAN200(1:5:end,2)'];
    ExcelData={VideoID,ImID,{CNN_Mix{repmat(1,1,NSamples)}},repmat(Epoc(k),1,NSamples),ClassData};
    for kC=1:length(ExcelData)
        xlswrite([OutExcelFile],[ExcelData{kC}]',[Column{kC},num2str(ExcelLine),[':' Column{kC}],num2str(ExcelLine+NSamples-1)]);
    end
   ExcelLine=ExcelLine+NSamples;
end

for k=1:length(Epoc)
    ClassData=[SigData.mixUNet21(1:5:end,2),SigData.mixUNet50(1:5:end,2),SigData.mixUNet200(1:5:end,2)];
    ExcelData={VideoID(1:5:end),ImID,{CNN_Mix{repmat(2,1,NSamples)}},repmat(Epoc(k),1,NSamples),ClassData};
    for kC=1:length(ExcelData)
        xlswrite([OutExcelFile],[ExcelData{kC}]',[Column{kC},num2str(ExcelLine),[':' Column{kC}],num2str(ExcelLine+NSamples-1)]);
    end
    ExcelLine=ExcelLine+NSamples;
end

for k=1:length(Epoc)
    ClassData=[SigData.mixVGG21(1:5:end,2),SigData.mixVGG50(1:5:end,2),SigData.mixVGG200(1:5:end,2)];
    ExcelData={VideoID,ImID,{CNN_Mix{repmat(3,1,NSamples)}},repmat(Epoc(k),1,NSamples),ClassData};
    for kC=1:length(ExcelData)
        xlswrite([OutExcelFile],[ExcelData{kC}]',[Column{kC},num2str(ExcelLine),[':' Column{kC}],num2str(ExcelLine+NSamples-1)]);
    end
    ExcelLine=ExcelLine+NSamples;
end

end

