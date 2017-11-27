clear;
fs=10000;
sin_ref = zeros(1,8000,'single');
cos_ref = zeros(1,8000,'single');

for i=1:40
    N=fs/(50*i);    %stevilo izracunanih vzorcev
    for r=1:200
        sin_ref(1,(r+((i-1)*200)))=sin(2*pi*(((50*i)/fs)*(r-1)));
        cos_ref(1,(r+((i-1)*200)))=cos(2*pi*(((50*i)/fs)*(r-1)));
    end
end
