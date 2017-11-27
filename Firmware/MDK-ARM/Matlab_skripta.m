fid = fopen('FFT_data.hex');
A   = fread(fid, Inf, 'uint8');
fclose(fid);
Fmt = repmat('%02X ', 1, 16);  % Create format string
Fmt(end) = '*';                % Trailing star
S   = sprintf(Fmt, A);         % One long string
C   = regexp(S, '*', 'split'); % Split at stars

