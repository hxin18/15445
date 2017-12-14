with countall as(
select p.race as race, count(p.case_id) as count
from parties as p, charges as c
where p.case_id = c.case_id
and p.race <> ''
and p.race in ('African American','Caucasian')
and c.disposition in ('Guilty','Not Guilty')
group by p.race
)
select p.race,c.disposition, count(p.case_id) * 100.0/countall.count
from parties as p, charges as c , countall
where p.case_id = c.case_id
and p.race = countall.race
and p.race in ('African American','Caucasian')
and c.disposition in ('Guilty','Not Guilty')
group by p.race,c.disposition;
