#pragma once

#include <memory>

#include "error.h"
#include "object.h"
#include "tokenizer.h"

Object* Read(Tokenizer* tokenizer, bool consider_all = false);
Object* ReadList(Tokenizer* tokenizer, bool consider_all = false);
