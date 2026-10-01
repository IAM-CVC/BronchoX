function [ SegParam ] = SetThreshold( SA, SegParam)

ThType=SegParam.ThType;
switch ThType
    case 'MainAirMode'
        [n,bins]=hist(SA(find(SA)),20);
        Th=bins(find(n==max(n)));
        %Th=mean(Th);
        Th=max(Th);
    case 'MainAirMean'
        Th=mean(SA(find(SA)));
    case 'MainAirMedian'
        Th=median(SA(find(SA)));
    case 'MainAirPercentile'
        Th=prctile(SA(find(SA)),SegParam.Prc);
        
    case 'Fixed'
        Th=SegParam.ThBronchiEner;
end

SegParam.Th=Th;


end

