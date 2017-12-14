with can_a as(
select count , name from (

select count(a.case_id) as count, name 
from attorneys as a, charges as c
where a.case_id =c.case_id
and name <> ''
group by name

)
where count >99
),


can_s as(
select a.name, count(a.case_id) as count 
from attorneys as a, charges as c, can_a
where a.case_id = c.case_id
and c.disposition = 'Not Guilty'
and a.name =can_a.name
group by a.name
)

select a.name,  a.count, s.count*100.0/a.count as pc
from can_a as a, can_s as s
where a.name = s.name
order by pc desc
limit 5
;
 

