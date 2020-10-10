#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

#include <pybind11/pybind11.h>

#include "seal/batchencoder.h"
#include "seal/biguint.h"
#include "seal/ciphertext.h"
#include "seal/ckks.h"
#include "seal/context.h"
#include "seal/decryptor.h"
#include "seal/encryptionparams.h"
#include "seal/encryptor.h"
#include "seal/evaluator.h"
#include "seal/intarray.h"
#include "seal/intencoder.h"
#include "seal/keygenerator.h"
#include "seal/memorymanager.h"
#include "seal/modulus.h"
#include "seal/plaintext.h"
#include "seal/publickey.h"
#include "seal/randomgen.h"
#include "seal/randomtostd.h"
#include "seal/relinkeys.h"
#include "seal/secretkey.h"
#include "seal/serialization.h"
#include "seal/valcheck.h"

using namespace seal;
using namespace std;

unique_ptr<Encryptor> encryptor;
unique_ptr<Evaluator> evaluator;
unique_ptr<Decryptor> decryptor;
unique_ptr<BatchEncoder> encoder;

namespace seal {

Ciphertext operator+(Ciphertext ct1, Ciphertext ct2) {
	Ciphertext result;
	evaluator->add(ct1, ct2, result);
	return result;
}

Ciphertext operator-(Ciphertext ct1, Ciphertext ct2) {
	Ciphertext result;
	evaluator->sub(ct1, ct2, result);
	return result;
}

Ciphertext operator*(const Ciphertext ct1, const Ciphertext ct2) {
	Ciphertext result;
	evaluator->multiply(ct1, ct2, result);
	return result;
}

}

#include "eigen3/unsupported/Eigen/CXX11/Tensor"
#include "Eigen/Dense"
#include "eigen3/Eigen/Core"

using namespace Eigen;

namespace Eigen {
template<>
struct NumTraits<Ciphertext>
{
    typedef Ciphertext Real;
    typedef Ciphertext NonInteger;
    typedef Ciphertext Nested;
  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 0,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};
}


void init() {
    EncryptionParameters parms(scheme_type::BFV);
    size_t poly_modulus_degree = 8192;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));

    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));
    auto context = SEALContext::Create(parms);
    KeyGenerator keygen(context);

    PublicKey public_key = keygen.public_key();
    SecretKey secret_key = keygen.secret_key();
    RelinKeys relin_keys = keygen.relin_keys_local();

    encryptor = unique_ptr<Encryptor>(new Encryptor(context, public_key));
    evaluator = unique_ptr<Evaluator>(new Evaluator(context));
    decryptor = unique_ptr<Decryptor>(new Decryptor(context, secret_key));

    encoder = unique_ptr<BatchEncoder>(new BatchEncoder(context));
}

Tensor<Ciphertext, 3> conv(Tensor<Ciphertext, 3> act, Tensor<Ciphertext, 4> weight, int stride);
Tensor<Ciphertext, 1> fc(Tensor<Ciphertext, 1> act, Tensor<Ciphertext, 2> weight);
void fill1(Tensor<Ciphertext, 1>& t);
void fill2(Tensor<Ciphertext, 2>& t);
void fill3(Tensor<Ciphertext, 3>& t);
void fill4(Tensor<Ciphertext, 4>& t);

Tensor<Ciphertext, 1> flatten(Tensor<Ciphertext, 3> t) {
	auto d = t.dimensions();

	Eigen::array<int, 1> new_dim{{d[0]*d[1]*d[2]}};
	Tensor<Ciphertext, 1> result = t.reshape(new_dim);

	return result;
}

Ciphertext sum3d(Tensor<Ciphertext, 3> t) {
	auto d = t.dimensions();
	Plaintext pt;
	Ciphertext base;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);
	encryptor->encrypt(pt, base);

	for(int i = 0; i < d[0]; i++)
		for(int j = 0; j < d[1]; j++)
			for(int k = 0; k < d[2]; k++)
				evaluator->add_inplace(base, t(i, j, k));

	return base;
}

Ciphertext sum1d(Tensor<Ciphertext, 1> t) {
	auto d = t.dimensions();
	Plaintext pt;
	Ciphertext base;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);
	encryptor->encrypt(pt, base);

	for(int i = 0; i < d[0]; i++)
		evaluator->add_inplace(base, t(i));

	return base;
}


Tensor<Ciphertext, 3> conv(Tensor<Ciphertext, 3> act, Tensor<Ciphertext, 4> weight, int stride) {
	auto weight_dimension = weight.dimensions();
	auto act_dimension = act.dimensions();
	size_t weight_output_channel_num = weight_dimension[0];
	size_t act_input_channel_num = act_dimension[0];
	
	// weight's windows should be NxN not MxN
	// calculate the result dimension
	if ((act_dimension[1] - weight_dimension[2]) % stride != 0) {
		std::cout << "Conv::Wrong stride!!!" << std::endl;
	}
	if ((act_dimension[2] - weight_dimension[3]) % stride != 0) {
		std::cout << "Conv::Wrong stride!!!" << std::endl;
	}

	size_t result_height = (act_dimension[1] - weight_dimension[2])/stride + 1;
	size_t result_width = (act_dimension[2] - weight_dimension[3])/stride + 1;
	
	Tensor<Ciphertext, 3> result(weight_output_channel_num, result_height, result_width);

	for(size_t i = 0; i < weight_output_channel_num; i++) {
		for(size_t j = 0; j < result_height; j++) {
			for(size_t k = 0; k < result_width; k++) {
				Eigen::array<int, 3> act_start = {0, stride*j, stride*k};
				Eigen::array<int, 3> act_size = \
					{act_input_channel_num, weight_dimension[2], weight_dimension[3]};
				Eigen::array<int, 4> weight_start = {i, 0, 0, 0};
				Eigen::array<int, 4> weight_size = \
					{1, act_input_channel_num, weight_dimension[2], weight_dimension[3]};
				Tensor<Ciphertext, 3> act_partial = act.slice(act_start, act_size);
				Tensor<Ciphertext, 4> weight_partial = weight.slice(weight_start, weight_size);
				Eigen::array<int, 3> new_dim{{act_input_channel_num, weight_dimension[2], weight_dimension[3]}};
				Tensor<Ciphertext, 3> weight_partial_reshape = weight_partial.reshape(new_dim);
				Tensor<Ciphertext, 3> res = act_partial * weight_partial_reshape;
				result(i, j, k) = sum3d(res);
			}
		}
	}

	return result;
}

Tensor<Ciphertext, 1> fc(Tensor<Ciphertext, 1> act, Tensor<Ciphertext, 2> weight) {
	auto d = weight.dimensions();
	size_t output_channel = d[1]; 
	size_t act_len = d[0];

	Tensor<Ciphertext, 1> result(output_channel);

	for(int i = 0; i < output_channel; i++) {
		Eigen::array<int, 2> weight_start = {0, i};
		Eigen::array<int, 2> weight_size = {act_len, 1};
		Tensor<Ciphertext, 2> weight_partial = weight.slice(weight_start, weight_size);
		Eigen::array<int, 1> new_dim{{act_len}};
		Tensor<Ciphertext, 1> weight_partial_reshape = weight_partial.reshape(new_dim);
		Tensor<Ciphertext, 1> res = act * weight_partial_reshape;
		result(i) = sum1d(res);
	}
	return result;
}

Tensor<Ciphertext, 3> square3(Tensor<Ciphertext, 3> t) {
	return t*t;
}

Tensor<Ciphertext, 1> square1(Tensor<Ciphertext, 1> t) {
	return t*t;
}

void fill4(Tensor<Ciphertext, 4>& t) {
	Plaintext pt;
	Ciphertext ct;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);	
	encryptor->encrypt(pt, ct);

	auto d = t.dimensions();
	for(int i = 0; i < d[0]; i++)
		for(int j = 0; j < d[1]; j++)
			for(int k = 0; k < d[2]; k++)
				for(int l = 0; l < d[3]; l++)
					t(i, j, k, l) = ct;
}

void fill3(Tensor<Ciphertext, 3>& t) {
	Plaintext pt;
	Ciphertext ct;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);	
	encryptor->encrypt(pt, ct);

	auto d = t.dimensions();
	for(int i = 0; i < d[0]; i++)
		for(int j = 0; j < d[1]; j++)
			for(int k = 0; k < d[2]; k++)
				t(i, j, k) = ct;
}

void fill2(Tensor<Ciphertext, 2>& t) {
	Plaintext pt;
	Ciphertext ct;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);	
	encryptor->encrypt(pt, ct);

	auto d = t.dimensions();
	for(int i = 0; i < d[0]; i++)
		for(int j = 0; j < d[1]; j++)
			t(i, j) = ct;
}

void fill1(Tensor<Ciphertext, 1>& t) {
	Plaintext pt;
	Ciphertext ct;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);	
	encryptor->encrypt(pt, ct);

	auto d = t.dimensions();
	for(int i = 0; i < d[0]; i++)
		t(i) = ct;
}

PYBIND11_MODULE(testOctNine, m) {
	pybind11::class_<Tensor<Ciphertext, 1>>(m, "Tensor1")
		.def(pybind11::init<>())
		.def(pybind11::init<int>());
	pybind11::class_<Tensor<Ciphertext, 2>>(m, "Tensor2")
		.def(pybind11::init<>())
		.def(pybind11::init<int, int>());
	pybind11::class_<Tensor<Ciphertext, 3>>(m, "Tensor3")
		.def(pybind11::init<>())
		.def(pybind11::init<int, int, int>());
	pybind11::class_<Tensor<Ciphertext, 4>>(m, "Tensor4")
		.def(pybind11::init<>())
		.def(pybind11::init<int, int, int, int>());
	m.def("init", &init);
	m.def("conv", &conv);
	m.def("flatten", &flatten);
	m.def("fc", &fc);
	m.def("fill1", &fill1);
	m.def("fill2", &fill2);
	m.def("fill3", &fill3);
	m.def("fill4", &fill4);
	m.def("square1", &square1);
	m.def("square3", &square3);
}
