function [ imaVOLROI ] = MaxPool3DBack( volRed,ratio,szeVol )

[hRed,wRed,nRed]=size(volRed);
imaVOLROI=zeros(szeVol);

for i=1:hRed
    for j=1:wRed
        for k=1:nRed
            
            imaVOLROI(ratio*(i-1)+1:min(ratio*i,szeVol(1)),ratio*(j-1)+1:min(szeVol(2),ratio*j),ratio*(k-1)+1:min(szeVol(3),ratio*k))=volRed(i,j,k);
        end
    end
end

end

