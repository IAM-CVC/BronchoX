function [F]=TracheaBronchiSep_Agnes(vol)

dist = bwdist(1-vol);

%volum filtrant les arees
F = zeros (size(vol));

for i=1:size(vol,3)

    %agafem la llesca en horitzonatl
    B = vol(:,:,i);
    D = dist(:,:,i);

    %fem un label per les zones
    [L,num] = bwlabel(B);

  

    %mirem quants labels hi ha
    n_l = 2;
    if (num>n_l) 

        %si hi ha més de dues arees busquem el valor de la D per cada label
        d_max_r = [];
        for k=1:num
            Lk = (L==k);
            DLk = Lk.*D;
            d_max_r = [d_max_r, max(DLk(:))];
        end
        %mirem els valors de les distancies i ens quedem amb les dues arees que
        %tenen major valor
        [v, p ] = sort (d_max_r, 'descend'); 
        a_filt = (L==p(1));
        for k=2:n_l
            Lk = (L==p(k));
            a_filt = max(a_filt,Lk);
        end

    %si n'hi ha menys 3 tres agafem la les zones binaries
    else 
        a_filt = B;
    end

    F(:,:,i) = a_filt;

end

%afafem la componnent connexa major
F = PrincipalConnComp(F,6,1);














