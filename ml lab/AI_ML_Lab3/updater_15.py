import json

nb_path = 'c:/Users/udayv/Desktop/5th sem/ml lab/AI_ML_Lab3/Student_AI_ML_Lab_.ipynb'
with open(nb_path, encoding='utf-8') as f:
    nb = json.load(f)

for i, cell in enumerate(nb['cells']):
    if cell['cell_type'] == 'code':
        source = ''.join(cell.get('source', []))
        if 'HPAI-BSC/medical-specialities' in source:
            nb['cells'][i]['source'] = [
                "!pip install datasets\n",
                "from datasets import load_dataset\n",
                "import torch\n",
                "from transformers import AutoTokenizer, AutoModelForCausalLM, TrainingArguments\n",
                "from peft import LoraConfig, get_peft_model\n",
                "from trl import SFTTrainer\n",
                "\n",
                "neurology_data = load_dataset(\"HPAI-BSC/medical-specialities\", \"Neurology\", split=\"test\")\n",
                "\n",
                "tokenizer = AutoTokenizer.from_pretrained(\"openai-community/gpt2\")\n",
                "tokenizer.pad_token = tokenizer.eos_token\n",
                "tokenizer.padding_side = \"right\"\n",
                "\n",
                "def map_to_text(example):\n",
                "    q = example['question']\n",
                "    ops = f\"1. {example['op1']}\\n2. {example['op2']}\\n3. {example['op3']}\\n4. {example['op4']}\"\n",
                "    ans = example.get(f\"op{example['cop']}\", \"\")\n",
                "    return {\"text\": f\"Question:\\n{q}\\n\\nOptions:\\n{ops}\\n\\nAnswer:\\n{ans}{tokenizer.eos_token}\"}\n",
                "\n",
                "neurology_data = neurology_data.map(map_to_text)\n",
                "\n",
                "model = AutoModelForCausalLM.from_pretrained(\"openai-community/gpt2\", device_map=\"auto\")\n",
                "\n",
                "inputs = tokenizer(neurology_data[0][\"text\"].split(\"\\n\\nAnswer:\\n\")[0] + \"\\n\\nAnswer:\\n\", return_tensors=\"pt\").to(\"cuda\" if torch.cuda.is_available() else \"cpu\")\n",
                "model.eval()\n",
                "base_output = model.generate(**inputs, max_new_tokens=50, pad_token_id=tokenizer.eos_token_id)\n",
                "base_text = tokenizer.decode(base_output[0], skip_special_tokens=True)\n",
                "clean_base = base_text.split(\"Answer:\\n\")[1].split(\"\\n\\n\")[0].strip() if \"Answer:\\n\" in base_text else base_text\n",
                "print(\"BASE MODEL OUTPUT:\\n\" + clean_base + \"\\n\")\n",
                "\n",
                "lora_config = LoraConfig(\n",
                "    r=32, \n",
                "    lora_alpha=32, \n",
                "    lora_dropout=0.05, \n",
                "    bias=\"none\", \n",
                "    task_type=\"CAUSAL_LM\",\n",
                "    target_modules=[\"c_attn\", \"c_proj\", \"c_fc\"]\n",
                ")\n",
                "model = get_peft_model(model, lora_config)\n",
                "\n",
                "training_args = TrainingArguments(\n",
                "    output_dir=\"./lora_med\", \n",
                "    per_device_train_batch_size=2, \n",
                "    gradient_accumulation_steps=2,\n",
                "    num_train_epochs=5, \n",
                "    learning_rate=2e-4\n",
                ")\n",
                "trainer = SFTTrainer(model=model, train_dataset=neurology_data, processing_class=tokenizer, peft_config=lora_config, args=training_args, dataset_text_field=\"text\")\n",
                "trainer.train()\n",
                "\n",
                "trainer.model.save_pretrained(\"./lora_med_adapter\")\n",
                "\n",
                "trainer.model.eval()\n",
                "ft_output = trainer.model.generate(**inputs, max_new_tokens=50, pad_token_id=tokenizer.eos_token_id)\n",
                "ft_text = tokenizer.decode(ft_output[0], skip_special_tokens=True)\n",
                "clean_ft = ft_text.split(\"Answer:\\n\")[1].split(\"\\n\\n\")[0].strip() if \"Answer:\\n\" in ft_text else ft_text\n",
                "print(\"FINETUNED MODEL OUTPUT:\\n\" + clean_ft)\n"
            ]

with open(nb_path, 'w', encoding='utf-8') as f:
    json.dump(nb, f, indent=1)

print("Medical cell parsing updated successfully.")
