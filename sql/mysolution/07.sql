 select  p.name, count(distinct(c.violation_county)) from parties as p, cases as c 
   where p.case_id = c.case_id   and p.type = "Defendant"
  and p.name <> ''
  group by p.name order by count(distinct(c.violation_county)) desc
  limit 3;

