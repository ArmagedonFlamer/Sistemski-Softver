#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <fstream>

using namespace std;

int memory_address=0;

struct section {
    string text;
    string name;
    string variable_usage;
    string variable_definition;
    int length;
    int start;
};

int precedence(char op){
    if(op == '+'||op == '-')
    return 1;
    if(op == '*'||op == '/')
    return 2;
    return 0;
}
 
// Function to perform arithmetic operations.
int applyOp(int a, int b, char op){
    switch(op){
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
}
 
// Function that returns value of
// expression after evaluation.
//preuzeto sa www.geeksforgeeks.org/expression-evaluation/
int evaluate(string tokens){
    int i;
     
    // stack to store integer values.
    stack <int> values;
     
    // stack to store operators.
    stack <char> ops;
     
    for(i = 0; i < tokens.length(); i++){
         
        // Current token is a whitespace,
        // skip it.
        if(tokens[i] == ' ')
            continue;
         
        // Current token is an opening
        // brace, push it to 'ops'
        else if(tokens[i] == '('){
            ops.push(tokens[i]);
        }
         
        // Current token is a number, push
        // it to stack for numbers.
        else if(isdigit(tokens[i])){
            int val = 0;
             
            // There may be more than one
            // digits in number.
            while(i < tokens.length() &&
                        isdigit(tokens[i]))
            {
                val = (val*10) + (tokens[i]-'0');
                i++;
            }
             
            values.push(val);
             
            // right now the i points to
            // the character next to the digit,
            // since the for loop also increases
            // the i, we would skip one
            //  token position; we need to
            // decrease the value of i by 1 to
            // correct the offset.
              i--;
        }
         
        // Closing brace encountered, solve
        // entire brace.
        else if(tokens[i] == ')')
        {
            while(!ops.empty() && ops.top() != '(')
            {
                int val2 = values.top();
                values.pop();
                 
                int val1 = values.top();
                values.pop();
                 
                char op = ops.top();
                ops.pop();
                 
                values.push(applyOp(val1, val2, op));
            }
             
            // pop opening brace.
            if(!ops.empty())
               ops.pop();
        }
         
        // Current token is an operator.
        else
        {
            // While top of 'ops' has same or greater
            // precedence to current token, which
            // is an operator. Apply operator on top
            // of 'ops' to top two elements in values stack.
            while(!ops.empty() && precedence(ops.top())
                                >= precedence(tokens[i])){
                int val2 = values.top();
                values.pop();
                 
                int val1 = values.top();
                values.pop();
                 
                char op = ops.top();
                ops.pop();
                 
                values.push(applyOp(val1, val2, op));
            }
             
            // Push current token to 'ops'.
            ops.push(tokens[i]);
        }
    }
     
    // Entire expression has been parsed at this
    // point, apply remaining ops to remaining
    // values.
    while(!ops.empty()){
        int val2 = values.top();
        values.pop();
                 
        int val1 = values.top();
        values.pop();
                 
        char op = ops.top();
        ops.pop();
                 
        values.push(applyOp(val1, val2, op));
    }
     
    // Top of 'values' contains result, return it.
    return values.top();
}

string itoa_hex(int x)  // IN HEX
{
    int tmp = x;

    int n = 5;
    string hexadecimalnum = "00 00\0";
    int i = 0;
    for (int j=0;j<4;++j)
    {
        if ((tmp & 0x0F) < 10)
            hexadecimalnum[n - 1 - i++]= (char) (48 + (tmp & 0x0F));
        else
            hexadecimalnum[n - 1 - i++] = 55 + (tmp & 0x0F);
        tmp >>= 4;
        if (i == 2)
            ++i;
    }
//    cout<<hexadecimalnum<<endl;
    return hexadecimalnum;
}

//find section by name
section* find_section(vector<section> *v, string name)
{
    vector<section>::iterator it = v->begin();
    int i=0;
    for(it; it!=v->end();++it)
    {
        if (it->name.compare(name)==0)
            return &(v->at(i));
        ++i;
    }
    return nullptr;
}

void check_global_variables(vector<pair<string,int>> *exported, vector<pair<string,int>> *imported)
{
    /*for(vector<pair<string,int>>::iterator it1 = exported->begin(); it1!=exported->end();++it1)
    {
        cout<<it1->first<<endl;
    }
    cout<<endl;*/
    for(vector<pair<string,int>>::iterator it = imported->begin(); it!=imported->end();++it)
    {
        string var = it->first;
        bool found = false;
   //     cout<<it->first<<endl;
        for(vector<pair<string,int>>::iterator it1 = exported->begin(); it1!=exported->end();++it1)
        {
            if (var.find(it1->first)!=string::npos)
                found=true;
        }
        if (!found)
        {
            cout<<"Cannot find variable: "<<var;
            exit(0);
        }
    }
}

void set_exported_value(vector<pair<string,int>> *exported, string definition_name, int definition_value)
{
    for (vector<pair<string,int>>::iterator it = exported->begin(); it!=exported->end();++it)
        if (definition_name.compare(it->first)==0)
        {
            it->second=definition_value;
            return;
        }
}


//ovo vazi za lokalne promenljive, globalne u posebnoj funkciji
void set_variables(vector<section> *code, vector<pair<string,int>> *exported)
{
    section *def_sec=nullptr;
    /*
    krecemo se kroz kod, prolazimo kroz definicije svaku sekciju, dodelimo vrednosti svakoj promenljivoj koja je jednaka
    zbiru memorijske adrese na kojoj sekcija pocinje i offseta promenljive, a zatim za svaku njenu definiciju proveravamo
    da li joj mozemo dodeliti vrednost
    */
    for (vector<section>::iterator it = code->begin(); it!=code->end();++it)
    {
        string definitions = it->variable_definition;
        int pos;
        //pripremam za --place opciju
        if (it->start==-1)
            it->start=memory_address;
        else
            memory_address=it->start;
        while((pos=definitions.find('\n'))!=string::npos)
        {
            string definition=definitions.substr(0,pos);
            definitions.erase(0,pos+1);
            //cout<<definition;
            string definition_file = definition.substr(1,definition.find("!")-1);
            string definition_section = definition.substr(definition.find("!!")+2, definition.find("#")-definition.find("!!")-2);
            string definition_name = definition.substr(definition.find("#")+1, definition.find(":")-definition.find("#")-1);
        //    cout<<definition_file<<" "<<definition_section<<" "<<definition_name<<endl;
            string section_name=definition_file;
            section_name.append("!").append(definition_section);
            section *tmp_sec = find_section(code, section_name);
            if (def_sec==nullptr)
                def_sec=tmp_sec;
            else
                if (def_sec->name.compare(tmp_sec->name)!=0)
                {
                    memory_address += def_sec->length;
                    def_sec=tmp_sec;
                }
            cout<<definition<<endl;
            int definition_value=memory_address+atoi(definition.substr(definition.find(":")+2, definition.length()-2-definition.find(":")-1).c_str()); // uzmi iz definitions-a
            
            set_exported_value(exported, definition_name, definition_value);

            //prati koliko karaktera je predjeno iz variables_usage kako bi, ukoliko se nadje match
            //mogli da obrisemo pojavljivanje iz variable_usage, posto je ono sad zamenjeno
            for (vector<section>::iterator it1 = code->begin(); it1!=code->end();++it1)
            {
                if (definition_file.compare(it1->name.substr(0,it1->name.find("!")))!=0)
                    continue;
                cout<<it1->name<<endl;
                string usages=it1->variable_usage;
                int current_pos=0;
                int pos1;
                while((pos1=usages.find('\n'))!=string::npos)
                {
                    string usage = usages.substr(0,pos1);
                    usages.erase(0,pos1+1);
                    /*
                        ako je naziv fajla isti i naziv promenljive isti, tu u kodu zameni 2 bajta sa trenutnom
                        memorijskom adresom pretvorenom u hex
                    */
                    string usage_file = usage.substr(1,usage.find("!")-1);
                    if (usage_file.compare(definition_file)==0)
                    {
                        string usage_name =  usage.substr(usage.find("#")+1, usage.find(":")-usage.find("#")-1);
                        if (usage_name.compare(definition_name)==0)
                        {
                            //nasli smo match
                            //u kodu na toj memoriji zameni 2B sa trenutnom memorijskom lokacijom
                            string section_name = usage.substr(1,usage.find("#")-1);
                            section *s = find_section(code,section_name);
                            //izvuci broj iz usage, na tom mestu brises 5 karaktera, i dodajes memory_Address+taj broj
                            string usage_num= usage.substr(usage.find(":")+2, usage.length()-usage.find(":")-3);
                            string new_value = itoa_hex(definition_value);
                            cout<<"menjam: "<<s->text.substr(int(atoi(usage_num.c_str())*3), 5)<<" u "<<new_value<<endl;
                            s->text.replace(int(atoi(usage_num.c_str())*3), 5, new_value);
                            it1->variable_usage.erase(current_pos,pos1+1);
                        //   s->text.replace( atoi(usage_num.c_str()), 5, new_value);
                        //   cout<<"ok";
                        }
                        else
                            current_pos+=pos1+1;
                    }
                    else
                        current_pos+=pos1+1;

                }
            }
        }
    }
}

void set_global_variables(vector<section> *code, vector<pair<string,int>> *exported)
{
    /*
    za svaku globalnu promenljivu, hocu da prodjem kroz sve fajlove
    i da umetnem njenu vrednost za one fajlove gde je ostalo koriscenje promenljive sa istim imenom
    */
    cout<<"GLOBAL"<<endl;
    for (vector<pair<string,int>>::iterator it = exported->begin(); it!=exported->end();++it)
    {
        for (vector<section>::iterator it1 = code->begin(); it1!=code->end();++it1)
        {
            //cout<<definition;
            string usages=it1->variable_usage;
            int pos1, current_pos=0;
            while((pos1=usages.find("\n"))!=string::npos)
            {
                string usage=usages.substr(0,pos1);
                usages.erase(0, pos1+1);
                string usage_file = usage.substr(1,usage.find("!")-1);
                string usage_section = usage.substr(usage.find("!")+1, usage.find("#")-usage.find("!")-1);
                string usage_name = usage.substr(usage.find("#")+1, usage.find(":")-usage.find("#")-1);
                string usage_num= usage.substr(usage.find(":")+2, usage.length()-usage.find(":")-3);
            //   cout<<usage_file<<" "<<usage_section<<" "<<usage_name<<" "<<usage_num<<endl;
            
                if (usage_name.compare(it->first)==0)
                {
                    string section_name=usage_file;
                    section_name.append("!").append(usage_section);
                    section *s = find_section(code, section_name);
                
                    string new_value = itoa_hex(it->second);
                    cout<<it->first<<" "<<it->second<<endl;
                    s->text.replace(int(atoi(usage_num.c_str())*3), 5, new_value);
                    it1->variable_usage.erase(current_pos,pos1+1);
                }
                else
                    current_pos+=pos1+1;
            }
        }
    }
}

void check_new_line(int i, std::ofstream& output)
{
    if (i%8==0)
    {
        output<<endl;
        string mem=itoa_hex(i);
        mem.erase(2,1);
        output<<mem<<": ";
    }
}

void print_output(vector<section>* code, string o)
{
    std::ofstream output;
    output.open(o);
    vector<section>::iterator it, it1=code->end();
    int i=0;
    output<<"0000: ";
    for(it=code->begin();it!=code->end();++it)
    {
        if (it1==code->end())
            it1=it;
        it1=it;
        string text = it->text;
        int pos;
        while((pos=text.find(" "))!=string::npos)
        {
            string byte=text.substr(0,pos);
            ++i;
            text.erase(0,pos+1);
            if ((byte.compare("30")==0)||(byte.compare("50")==0)||(byte.compare("51")==0)||(byte.compare("52")==0)||(byte.compare("53")==0)||
                (byte.compare("A0")==0)||(byte.compare("B0")==0))
            {
                output<<byte<<" ";
                check_new_line(i,output);
                pos=text.find(" ");
                byte=text.substr(0,pos);
                ++i;
                text.erase(0,pos+1);
                output<<byte<<" ";
                if (byte.find("7")==1)
                { 
                    pos=text.find(" ");
                    byte=text.substr(0,pos);
                    ++i;
                    text.erase(0,pos+1);
                    output<<byte<<" ";
                    check_new_line(i,output);
                    short jump_address;
                    pos=text.find(" ");
                    byte=text.substr(0,pos);
                    text.erase(0,pos+1);
                    jump_address=((int)strtol(byte.c_str(), NULL, 16)<<8);
                    pos=text.find(" ");
                    byte=text.substr(0,pos);
                    text.erase(0,pos+1);
                    jump_address|=(int)strtol(byte.c_str(), NULL, 16);
                    jump_address-=(i+2);
                    string relative_address = itoa_hex(jump_address);
                    cout<<itoa_hex(jump_address)<<endl;
                    output<<relative_address.substr(0,2)<<" ";
                    ++i;
                    check_new_line(i,output);
                    output<<relative_address.substr(3,5)<<" ";
                    ++i;
                    check_new_line(i,output);
                }
            }
            else
                output<<byte<<" ";
            check_new_line(i,output);
        }
    }
    if ((i%8)!=0)
    {
        while((i%8)!=0)
        {
            output<<"00 ";
            ++i;
        }
        output<<endl;
    }
    output.close();
}

void sort_sections(vector<section> *code)
{
    for (int i=0;i<code->size();++i)
        for (int j=i+1;j<code->size();++j)
        {
           section tmp1 = code->at(i);
           section tmp2 = code->at(j);

           if ((tmp2.start!=-1)&&((tmp1.start==-1)||(tmp1.start>tmp2.start)))
            {
                (*code)[i]=tmp2;
                (*code)[j]=tmp1;
                for(int k=i+1;k<j;++k)
                {
                    tmp1 = code->at(j);
                    tmp2 = code->at(k);
                    (*code)[j]=tmp2;
                    (*code)[k]=tmp1;
                }
            }
        }
    //ukoliko postoje rupe izmedju sekcija sa predefinisanim pocetkom, umetnuti sekcije koje
    //su manje od postojecih rupa izmedju tih sekcija
    int mem=0; //memorijska lokacija na kojoj se zavrsava poslednja sekcija
    for (int i=0;i<code->size();++i)
    {
        if (((*code)[i].start-mem)!=0)
        {
            int gap = (*code)[i].start-mem;
            for (int j=i+1;j<code->size();++j)
            {
                section tmp1 = code->at(i);
                section tmp2 = code->at(j);
                
                //ako je duzina sekcije manja od postojece rupe, ubaci tu sekciju
                if ((tmp2.start==-1)&&(tmp2.length<gap))
                    {
                        tmp2.start=mem;
                        mem+=tmp2.length;
                        gap-=tmp2.length;
                        (*code)[i]=tmp2;
                        (*code)[j]=tmp1;
                        for(int k=i+1;k<j;++k)
                        {
                            tmp1 = code->at(j);
                            tmp2 = code->at(k);
                            (*code)[j]=tmp2;
                            (*code)[k]=tmp1;
                        }
                        //posto sam zamenio i sa j, moram pomeriti i da pokazuje na pocetni element
                        ++i;
                    }
            }
        }
        mem=(*code)[i].start+(*code)[i].length;
    }
};

void calculate_expressions(vector<section> *code, vector<pair<string,string>> *expressions)
{
    for (vector<pair<string,string>>::iterator it = expressions->begin(); it!=expressions->end();++it)
    {
        cout<<it->first<<" "<<it->second<<endl;
        //nadji vrednosti promenljivih u izrazu
        int pos;
        string exp=it->second;
        string expression="";
        while(((pos=exp.find(" "))!=string::npos))
        {
            string variable=exp.substr(0,pos);
            exp.erase(0,pos+1);
            if (pos==1)
            {
                expression.append(" ");
                expression.append(variable);
            }
            else
            {
                for (vector<section>::iterator it1=code->begin(); it1!=code->end();++it1)
                {
                    string definitions = it1->variable_definition;
                    int pos1, found_variable=0;
                    while((!found_variable)&&((pos1=definitions.find("\n"))!=string::npos))
                    {
                        string definition = definitions.substr(0,pos1);
                        definitions.erase(0,pos1+1);
                        if(variable.compare(definition.substr(definition.find("#")+1, definition.find(":")-definition.find("#")-1))==0)
                        {
                            cout<<variable<<" "<<definition.substr(definition.find(":")+2, definition.length()-1-definition.find(":")-2)<<endl;
                            found_variable=1;
                            expression.append(" ");
                            expression.append(definition.substr(definition.find(":")+2, definition.length()-1-definition.find(":")-2));
                        }   
                    }
                    if (found_variable)
                        break;
                }
            }
        }
        int value=evaluate(expression);
        string variable=it->first.substr(it->first.find("#")+1, it->first.find(":")-it->first.find("#")-1);
        cout<<variable<<" "<<value<<endl;
        //zameniti u kodu gde god da se pojavljuje promenljiva ciju smo vrednost sada izracunali
        for (vector<section>::iterator it1=code->begin(); it1!=code->end();++it1)
        {
            string usages = it1->variable_usage;
            int pos1,current_pos=0;
            while((pos1=usages.find("\n"))!=string::npos)
            {
                string usage = usages.substr(0,pos1);
                usages.erase(0,pos1+1);
                string usage_file = usage.substr(1,usage.find("!")-1);
                string usage_section = usage.substr(usage.find("!")+1, usage.find("#")-usage.find("!")-1);
                string usage_name = usage.substr(usage.find("#")+1, usage.find(":")-usage.find("#")-1);
                string usage_num= usage.substr(usage.find(":")+2, usage.length()-usage.find(":")-3);
                cout<<usage_section<<" "<<usage_name<<" "<<usage_num<<" ";
                if(variable.compare(usage_name)==0)
                {
                    cout<<variable<<" "<<usage_num<<endl;
                    string section_name=usage_file;
                    section_name.append("!").append(usage_section);
                    section *s = find_section(code, section_name);
                
                    string new_value = itoa_hex(value);
                    s->text.replace(int(atoi(usage_num.c_str())*3), 5, new_value);
                    it1->variable_usage.erase(current_pos,pos1+1);  
                }
                else
                    current_pos+=pos1+1;
            }
        }

    }
}   

void set_equ_variables(vector<section> *code,vector<pair<string,int>> *equ_vals)
{
     for (vector<pair<string,int>>::iterator it = equ_vals->begin(); it!=equ_vals->end();++it)
    {
        for (vector<section>::iterator it1 = code->begin(); it1!=code->end();++it1)
        {
            string usages=it1->variable_usage;
            int pos1, current_pos=0;
            while((pos1=usages.find("\n"))!=string::npos)
            {
                string usage=usages.substr(0,pos1);
                usages.erase(0, pos1+1);
                string usage_file = usage.substr(1,usage.find("!")-1);
                string usage_section = usage.substr(usage.find("!")+1, usage.find("#")-usage.find("!")-1);
                string usage_name = usage.substr(usage.find("#")+1, usage.find(":")-usage.find("#")-1);
                string usage_num= usage.substr(usage.find(":")+2, usage.length()-usage.find(":")-3);
            //   cout<<usage_file<<" "<<usage_section<<" "<<usage_name<<" "<<usage_num<<endl;
            
                if (usage_name.compare(it->first)==0)
                {
                    string section_name=usage_file;
                    section_name.append("!").append(usage_section);
                    section *s = find_section(code, section_name);
                
                    int new_value = it->second;
                    cout<<it->first<<" "<<it->second<<endl;
                    s->text.replace(int(atoi(usage_num.c_str())*3), 5, itoa_hex(new_value));
                    it1->variable_usage.erase(current_pos,pos1+1);
                }
                else
                    current_pos+=pos1+1;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    vector<section> *code= new vector<section>();
    vector<pair<string,int>> *exported= new vector<pair<string,int>>();
    vector<pair<string,int>> *imported= new vector<pair<string,int>>();
    vector<pair<string,string>> *expressions= new vector<pair<string,string>>();
    vector<pair<string, int>>  *equ_vals = new vector<pair<string, int>>();
    section *tmp=nullptr;
    int pos;
    string output = "";
    for(int i=1;i<argc;++i)
    {
        string input=*new string(argv[i]);
        int pos;
        if (input.compare("-o")==0)
        {
            cout<<"-o OPCIJA"<<endl;
            ++i;
            output=*new string(argv[i]);
        }
        else
            if ((pos=input.find("-place="))!=string::npos)  
            {
                cout<<"-place OPCIJA"<<endl;
            }   
            else
            {

                std::ifstream file(input);
                std::string str; 
                while (std::getline(file, str))
                {
                    // ako ne postoji razmak => pocinje nova sekcija
                    cout<<str<<endl;
                    if(str.length()==0)
                        continue;
                    if (str.find(' ')==string::npos)
                    {
                        tmp = new section();
                        string section_name= *new string(argv[i]);
                        section_name.append("!").append(str);
                        tmp->name=section_name;
                        tmp->start=-1;
                        code->insert(code->end(), *tmp);
                        tmp=&code->at(code->size()-1);
                    }
                    else
                        if((pos = str.find("#:"))!=string::npos)
                        {
                            string section_name= *new string(argv[i]);
                            section_name.append("!").append(str.substr(1, pos-1));
                            tmp= find_section(code,section_name);
                            str.erase(0,pos+3);
                            str.erase(str.length()-1,1);
                            tmp->length= stoi(str);
                        }
                        else
                            //izvezene globalne promenljive
                            if((pos = str.find("!#"))!=string::npos)
                            {
                                exported->insert(exported->end(), *new pair<string,int>(str.substr(3, str.find(":")-3),0));
                            }
                            else
                            //uvozenje globalne promenljive
                                if((pos = str.find("#!"))!=string::npos)
                                {
                                    imported->insert(imported->end(), *new pair<string,int>(str.substr(3, str.find(":")-3),0));
                                }
                                else
                                //definisanje promenljive
                                    if ((pos = str.find("!!"))!=string::npos)
                                    {
                                        string section_name= *new string(argv[i]);
                                        section_name.append("!").append(str.substr(str.find("!!")+2, str.find("#")-str.find("!!")-2));
                                        section *s = find_section(code,section_name);
                                        if (s==nullptr)
                                            cout<<str<<endl;
                                        else
                                            s->variable_definition.append(str.append("\n"));
                                    }
                                    else
                                        if ((pos = str.find("?"))!=string::npos)
                                        {
                                            expressions->insert(expressions->end(), *new pair<string,string>(str.substr(1,str.find(":")), str.substr(str.find(":")+2, str.length()-1-str.find(":")-2)));
                                        }
                                        else
                                            if(((str.at(0)=='{')&&str.find("#")==string::npos))
                                                {   //radi se o vrednosti dodeljenoj preko .equ
                                                    cout<<"x"<<str.substr(str.find(":")+2,str.length()-str.find(":")-3)<<"x";
                                                    equ_vals->insert(equ_vals->end(),*new pair<string,int>(str.substr(1,str.find(":")-1), stoi(str.substr(str.find(":")+2,str.length()-str.find(":")-3))));
                                                }
                                            else
                                                if (str.at(0)=='{')
                                                {
                                                    string section_name= *new string(argv[i]);
                                                    section_name.append("!").append(str.substr(str.find("!")+1, str.find("#")-str.find("!")-1));
                                                    section *s = find_section(code,section_name);
                                                    if (s==nullptr)
                                                        cout<<str<<endl;
                                                    else
                                                        s->variable_usage.append(str.append("\n"));
                                                }
                                                else
                                                    tmp->text.append(str);
                }
                file.close();
            }
    }

    for(int i=0;i<argc;++i)
    {
        string input = *new string(argv[i]);
        if ((pos=input.find("-place="))!=string::npos)  
            {
                cout<<"-place OPCIJA"<<endl;
                input.erase(0,7);
                pos = input.find("@");
                string section_name=input.substr(0,pos);
                //brisem @0x
                input.erase(0,pos+3);
                int start = stoi(input.c_str());
                vector<section>::iterator it = code->begin();
                int i=0;
                for(it; it!=code->end();++it)
                {
                    string tmp = it->name;
                    tmp.erase(0,tmp.find("!")+1);
                    if (tmp.compare(section_name)==0)
                    {
                        cout<<"ODRADIO PLACE, sekcija "<<tmp<<" pocinje na "<<start<<endl;
                        it->start=start;
                    }
                }
            }   
    }
    
    vector<section>::iterator it = code->begin();
    //sortiram po start poljima sekcija, rastuce (negativne vrednosti se guraju na kraj)
    //ako je mem_address+section_len<start trenutne sekcije, ubaci je ispred te sekcije
    for(it=code->begin();it!=code->end();++it)
    { cout<<it->name<<" "<<it->length<<endl<<it->text<<endl<<it->variable_usage<<endl;}
    sort_sections(code);
    for(it=code->begin();it!=code->end();++it)
    { cout<<it->name<<" "; printf("%02X %02X\n",it->length,it->start);}
    check_global_variables(exported, imported);
    set_equ_variables(code,equ_vals);
    set_variables(code, exported);
    set_global_variables(code, exported);
    calculate_expressions(code, expressions);
    print_output(code,output);
    cout<<endl;
    for (vector<pair<string,int>>::iterator it1=exported->begin();it1!=exported->end();++it1)
        {cout<<it1->first<<" : ";printf("%02X\n",it1->second);}
    cout<<endl;
    for(it=code->begin();it!=code->end();++it)
        { cout<<it->name<<" "; printf("%02X %02X\n",it->length,it->start);}
    return 0;
}