select case_id, filing_date as date from cases where date > '1949-12-31' and date < '1960-01-01' order by date asc limit 3;
