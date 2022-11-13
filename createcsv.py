import pandas as pd
from faker import Faker

fake = Faker()
df = pd.DataFrame(columns=["Student ID", "First Name", "Last Name", "CNIC", "Email", "Phone Number", "Credit Hours", "CGPA"])
for i in range(1000):
    id=fake.unique.pystr_format("20K-####")                                         #string
    fname=fake.first_name()                                                         #string
    lname=fake.last_name()                                                          #string
    # % means non-zero digit since CNIC's first digit is never 0
    cnic=str(fake.unique.pystr_format("%####-#######-#"))                           #string
    email=fake.email().split('@')[0]+"@gmail.com"                                   #string
    phone=f'+92 {fake.msisdn()[3:]}'                                                #string
    # 1st semester has 17 credit hrs and to earn degree 130 are needed
    crd=fake.pyint(min_value=17, max_value=130)                                     #int
    cgpa=fake.pyfloat(left_digits=1, right_digits=2, min_value=2.00, max_value=4.00)#float
    df.loc[len(df.index)]=[id,fname,lname,cnic,email,phone,crd,cgpa]
df.to_csv("20Kstudentrecords.csv",index=False)