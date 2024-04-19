
cd 00-let_lang        
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 01-pro_lang                     
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 02-nameless                     
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 03-letrec_lang              
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 04-explicit_refs                
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 05-implicit-refs    
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 06-mutable_pairs    
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 07-call_by_need     
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 08-letrec_lang_cont 
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 09-letrec_lang_stack
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 10-exception        
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 11-exception_stack  
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 12-thread_lang      
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 13-thread_lang_stack
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 14-thread_lang_time
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 15-thread_lang_time_stack
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 16-simple_modules
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 17-classes
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..

cd 18-checked
source ./build.sh
./interpretor.exe
rm ./interpretor.exe
cd ..